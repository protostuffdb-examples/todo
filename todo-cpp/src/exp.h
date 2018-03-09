#pragma once

#include "../g/user/index_generated.h"

#include "user/todo.h"

/*
static void printTodos(void* flatbuf)
{
    auto wrapper = flatbuffers::GetRoot<todo::user::Todo_PList>(flatbuf);
    auto plist = wrapper->p();
    fprintf(stdout, "%d todo(s)\n", plist->Length());
    for (auto it = plist->begin(); it != plist->end(); ++it)
        fprintf(stdout, "  key: %s, title: %s\n", it->key()->c_str(), it->title()->c_str());
}

nana::listbox::oresolver& operator << (nana::listbox::oresolver& orr, const todo::user::Todo* todo)
{
    orr << todo->title()->c_str();
    orr << (todo->completed() ? "*" : " ");
    return orr;
}
*/

static const int
        // page
        PAGE_SIZE = 20,
        MULTIPLIER = 2,
        // listbox
        LB_OUTER = util::MARGIN * 2,
        LB_HEIGHT = util::HEIGHT - LB_OUTER,
        LB_WIDTH = util::WIDTH - LB_OUTER,
        // panel
        LB_PANEL_WIDTH = LB_WIDTH - (util::MARGIN * 3),
        // inner
        LB_FIELDS = 2,
        LB_INNER = util::MARGIN * 3 * LB_FIELDS,
        COMPLETED_WIDTH = 20,
        TITLE_WIDTH = LB_WIDTH - LB_INNER - COMPLETED_WIDTH;

// hack
struct TodoItem;
static std::vector<TodoItem*> todo_items;

using TodoStore = coreds::PojoStore<todo::Todo, todo::user::Todo>;

struct TodoItem : nana::listbox::inline_notifier_interface
{
    std::function<void()> $selected{ std::bind(&TodoItem::selected, this) };
    
    inline_indicator* ind_ { nullptr };
    index_type pos_;
    
    ui::DeferredPanel pnl_ {
        "margin=[1,10]"
        "<title_>"
        "<ts_ weight=120 margin=[0,5]>"
        "<txt_ weight=200 margin=[0,5,0,0]>"
        "<btn_ weight=25>" 
    };
    nana::label title_;
    nana::label ts_;
    nana::textbox txt_;
    nana::button btn_;
    
    todo::Todo* pojo{ nullptr };
    TodoStore* store{ nullptr };
    int idx;
    
    TodoItem()
    {
        todo_items.push_back(this);
    }
    void init(int idx, TodoStore* store, std::function<void(const nana::arg_keyboard& arg)> $navigate)
    {
        this->idx = idx;
        this->store = store;
        
        title_.events().key_press($navigate);
        ts_.events().key_press($navigate);
    }
    void selected()
    {
        ind_->selected(pos_);
    }
    void update(todo::Todo* message)
    {
        pojo = message;
        if (message == nullptr)
        {
            pnl_.hide();
            return;
        }
        
        title_.caption(pojo->title);
        
        std::string timeago;
        timeago.reserve(16); // just moments ago
        coreds::util::appendTimeagoTo(timeago, pojo->ts);
        ts_.caption(timeago);
        
        pnl_.show();
    }
private:
    void create(nana::window wd) override
    {
        pnl_.create(wd);
        pnl_.hide();
        
        // title
        title_.create(pnl_);
        title_.transparent(true)
            .events().click($selected);
        pnl_.place["title_"] << title_;
        
        // ts
        ts_.create(pnl_);
        ts_.transparent(true)
            .text_align(nana::align::right)
            .events().click($selected);
        pnl_.place["ts_"] << ts_;
        
        // textbox
        txt_.create(pnl_);
        txt_.events().click($selected);
        pnl_.place["txt_"] << txt_;
        
        // button
        btn_.create(pnl_);
        btn_.caption("x");
        btn_.events().click([this]
        {
            ind_->selected(pos_);
            // TODO delete the item when button is clicked
            //auto& lsbox = dynamic_cast<nana::listbox&>(ind->host());
            //lsbox.erase(lsbox.at(pos_));
        });
        pnl_.place["btn_"] << btn_;
    }
    void notify_status(status_type status, bool on) override
    {
        if (pojo && on && status == status_type::selecting)
            store->select(idx);
    }
    void activate(inline_indicator& ind, index_type pos) override { ind_ = &ind; pos_ = pos; }
    void resize(const nana::size& d) override { pnl_.size(d); }
    void set(const std::string& value) override {}
    bool whether_to_draw() const override { return false; }
};

static const std::string SORT_TOGGLE[] = {
    " <color=0x0080FF size=11 target=\"0\"> dsc </>",
    " <color=0x0080FF size=11 target=\"1\"> asc </>",
};

struct Home : ui::Panel, util::HasState<bool>
{
    TodoStore store;
private:
    std::function<void(void* res)> $onResponse{
        std::bind(&Home::onResponse, this, std::placeholders::_1)
    };
    std::function<void()> $beforePopulate{
        std::bind(&Home::beforePopulate, this)
    };
    std::function<void(const nana::arg_keyboard& arg)> $navigate{
        std::bind(&Home::navigate, this, std::placeholders::_1)
    };
    std::function<void(nana::label::command cmd, const std::string& target)> $onLabelEvent{
        std::bind(&Home::onLabelEvent, this, std::placeholders::_1, std::placeholders::_2)
    };
    
    nana::textbox search_{ *this };
    
    nana::label add_{ *this,
        "  "
        "<bold color=0x0080FF size=11 target=\"2\"> + </>"
    };
    nana::label sort_{ *this, SORT_TOGGLE[0] };
    nana::label refresh_{ *this,
        "  "
        "<color=0x0080FF size=11 target=\"3\">refresh</>"
    };
    
    ui::BgPanel msg_panel_{ *this,
        "margin=[3,2,1,3]"
        "<msg_>"
        "<msg_close_ weight=20>",
        0xFFFFFF
    };
    nana::label msg_{ msg_panel_, "" };
    nana::label msg_close_{ msg_panel_, "<bold target=\"8\"> x </>" };
    
    nana::label page_info_{ *this, "" };
    
    nana::label nav_{ *this,
        "<color=0x0080FF size=11 target=\"4\">\\<\\<</>"
        "     "
        "<color=0x0080FF size=11 target=\"5\"> \\< </>"
        "     "
        "<color=0x0080FF size=11 target=\"6\"> \\> </>"
        "     "
        "<color=0x0080FF size=11 target=\"7\">\\>\\></>"
    };
    
    nana::listbox list_{ *this, { 0, 25 + util::MARGIN, unsigned(LB_WIDTH), unsigned(LB_HEIGHT - (25 + util::MARGIN)) } };
    
    std::string page_str;
    
    bool initialized{ false };
    
    int item_offset;
    
public:
    Home(ui::Panel& owner, std::vector<util::HasState<bool>*>& container,
            const char* field, bool active = false) : ui::Panel(owner,
        "vert"
        "<horizontal weight=25"
          "<search_ weight=200>"
          "<add_ weight=40>"
          "<sort_ weight=40>"
          "<refresh_ weight=80>"
          "<msg_panel_>"
          "<page_info_ weight=160>"
          "<nav_ weight=160>"
        ">"
        "<list_>"
    )
    {
        container.push_back(this);
        place["search_"] << search_.tip_string("Todo");
        
        // ctrls
        place["add_"] << add_
                .text_align(nana::align::center)
                .add_format_listener($onLabelEvent)
                .format(true);
        
        place["sort_"] << sort_
                .text_align(nana::align::center)
                .add_format_listener($onLabelEvent)
                .format(true);
        
        place["refresh_"] << refresh_
                .text_align(nana::align::center)
                .add_format_listener($onLabelEvent)
                .format(true);
        
        // =====================================
        // msg
        
        place["msg_panel_"] << msg_panel_;
        
        msg_panel_.place["msg_"] << msg_
                .text_align(nana::align::left)
                .add_format_listener($onLabelEvent)
                .transparent(true);
        msg_panel_.place["msg_close_"] << msg_close_
                .text_align(nana::align::right)
                .add_format_listener($onLabelEvent)
                .format(true)
                .transparent(true);
        msg_close_.fgcolor(ui::MsgColors::DEFAULT.close_fg);
        
        msg_panel_.place.collocate();
        
        // =====================================
        
        place["page_info_"] << page_info_
                .text_align(nana::align::right);
        
        place["nav_"] << nav_
                .text_align(nana::align::right)
                .add_format_listener($onLabelEvent)
                .format(true);
        
        // listbox
        list_.show_header(false);
        list_.enable_single(true, true);
        
        // 1-column inline widgets
        list_.append_header("", LB_PANEL_WIDTH);
        list_.at(0).inline_factory(0, nana::pat::make_factory<TodoItem>());
        
        // 2-column text-only
        //list_.append_header("", TITLE_WIDTH);
        //list_.append_header("", COMPLETED_WIDTH);
        
        place["list_"] << list_;
        place.collocate();
        
        // initially hidden
        msg_panel_.hide();
        place.field_visible("list_", false);
        
        owner.place[field] << *this;
        owner.place.field_display(field, active);
    }
    void update(bool on) override
    {
        if (on)
            store.fetchUpdate();
    }
    void show(const std::string& msg, ui::Msg type = ui::Msg::$ERROR)
    {
        msg_.caption(msg);
        
        switch (type)
        {
            case ui::Msg::$SUCCESS:
                msg_.fgcolor(ui::MsgColors::DEFAULT.success_fg);
                msg_panel_.bgcolor(ui::MsgColors::DEFAULT.success_bg);
                break;
            case ui::Msg::$ERROR:
                msg_.fgcolor(ui::MsgColors::DEFAULT.error_fg);
                msg_panel_.bgcolor(ui::MsgColors::DEFAULT.error_bg);
                break;
            case ui::Msg::$WARNING:
                msg_.fgcolor(ui::MsgColors::DEFAULT.warning_fg);
                msg_panel_.bgcolor(ui::MsgColors::DEFAULT.warning_bg);
                break;
        }
        
        ui::visible(msg_, true);
        ui::visible(msg_close_, true);
        msg_panel_.show();
    }
    
private:
    void lazyInit()
    {
        item_offset = todo_items.size();
        
        auto slot = list_.at(0);
        for (int i = 0; i < PAGE_SIZE; ++i)
            slot.append({ "" });
        
        // the stmt below creates the items
        place.field_visible("list_", true);
        place.field_visible("list_", false);
        
        for (int i = 0; i < PAGE_SIZE; ++i)
            todo_items[item_offset + i]->init(i, &store, $navigate);
    }
    void populate(int idx, todo::Todo* pojo)
    {
        if (!initialized)
        {
            lazyInit();
            initialized = true;
        }
        todo_items[item_offset + idx]->update(pojo);
    }
    void select(int idx)
    {
        if (idx != -1)
            todo_items[item_offset + idx]->selected();
        else if (nullptr != store.getSelected())
            list_.at(0).select(false);
    }
    void beforePopulate()
    {
        place.field_visible("list_", false);
    }
    void afterPopulate(int selectedIdx)
    {
        select(selectedIdx);
        place.field_visible("list_", true);
        
        page_str.clear();
        store.appendPageInfoTo(page_str);
        page_info_.caption(page_str);
    }
    void afterPopulate()
    {
        afterPopulate(store.getSelectedIdx());
    }
    void onLabelEvent(nana::label::command cmd, const std::string& target)
    {
        if (nana::label::command::click != cmd)
            return;
        
        int i = std::atoi(target.c_str());
        switch (i)
        {
            case 0:
            case 1:
                store.toggleDesc();
                break;
            case 3: // refresh
                store.fetchUpdate();
                break;
            case 4:
                store.pageTo(0);
                break;
            case 5:
                if (0 == (i = store.getPage()))
                    store.fetch(store.isDesc());
                else
                    store.pageTo(i - 1);
                break;
            case 6:
                if (store.getPageCount() == (i = store.getPage()))
                    store.fetch(!store.isDesc());
                else
                    store.pageTo(i + 1);
                break;
            case 7:
                store.pageTo(store.getPageCount());
                break;
            case 8:
                msg_panel_.hide();
                break;
        }
    }
    void navigate(const nana::arg_keyboard& arg)
    {
        int idx = store.getSelectedIdx();
        switch (arg.key)
        {
            case nana::keyboard::os_arrow_up:
                if (arg.ctrl)
                {
                    select(0);
                }
                else if (-1 == idx)
                {
                    select(store.getVisibleCount() - 1);
                }
                else if (0 != idx)
                {
                    select(idx - 1);
                }
                else if (0 != store.getPage())
                {
                    store.pageTo(store.getPage() - 1, $beforePopulate);
                    afterPopulate(store.getVisibleCount() - 1);
                }
                break;
            case nana::keyboard::os_arrow_down:
                if (arg.ctrl)
                {
                    select(store.getVisibleCount() - 1);
                }
                else if (-1 == idx)
                {
                    select(0);
                }
                else if (++idx != store.getVisibleCount())
                {
                    select(idx);
                }
                else if (store.getPageCount() != store.getPage())
                {
                    store.pageTo(store.getPage() + 1, $beforePopulate);
                    afterPopulate(0);
                }
                break;
            case nana::keyboard::os_arrow_left:
                if (arg.ctrl)
                    store.pageTo(0);
                else if (0 == store.getPage())
                    store.fetch(store.isDesc());
                else
                    store.pageTo(store.getPage() - 1);
                break;
            case nana::keyboard::os_arrow_right:
                if (arg.ctrl)
                    store.pageTo(store.getPageCount());
                else if (store.getPageCount() == store.getPage())
                    store.fetch(!store.isDesc());
                else
                    store.pageTo(store.getPage() + 1);
                break;
            case nana::keyboard::space:
                if (arg.ctrl)
                    store.fetchUpdate();
                else if (arg.shift)
                    store.toggleDesc();
                break;
        }
    }
    void onResponse(void* res)
    {
        if (res == nullptr)
        {
            nana::internal_scope_guard lock;
            store.cbFetchFailed();
            show(store.errmsg);
        }
        else
        {
            store.cbFetchSuccess(flatbuffers::GetRoot<todo::user::Todo_PList>(res)->p());
        }
    }
public:
    void init(coreds::Opts opts, util::RequestQueue& rq)
    {
        store.init(opts);
        store.$fnKey = [](const todo::Todo& pojo) {
            return pojo.key.c_str();
        };
        store.$fnKeyFB = [](const todo::user::Todo* message) {
            return message->key()->c_str();
        };
        store.$fnUpdate = [](todo::Todo& pojo, const todo::user::Todo* message) {
            message->title()->assign_to(pojo.title);
            pojo.completed = message->completed();
        };
        store.$fnPopulate = [this](int idx, todo::Todo* pojo) {
            populate(idx, pojo);
        };
        store.$fnCall = [this](std::function<void()> op) {
            nana::internal_scope_guard lock;
            beforePopulate();
            op();
            afterPopulate();
        };
        store.$fnEvent = [this](coreds::EventType type, bool on) {
            switch (type)
            {
                case coreds::EventType::DESC:
                {
                    nana::internal_scope_guard lock;
                    sort_.caption(SORT_TOGGLE[on ? 0 : 1]);
                    break;
                }
                case coreds::EventType::LOADING:
                    // hide errmsg when loading
                    if (on)
                    {
                        nana::internal_scope_guard lock;
                        ui::visible(msg_, false);
                        ui::visible(msg_close_, false);
                    }
                    break;
                case coreds::EventType::VISIBLE:
                {
                    nana::internal_scope_guard lock;
                    if (on)
                        select(store.getSelectedIdx());
                    
                    place.field_visible("list_", on);
                    
                    page_str.clear();
                    store.appendPageInfoTo(page_str);
                    page_info_.caption(page_str);
                    break;
                }
            }
        };
        store.$fnFetch = [this, &rq](coreds::ParamRangeKey prk) {
            std::string buf;
            prk.stringifyTo(buf);
            
            rq.queue.emplace("/todo/user/Todo/list", buf, "Todo_PList", &store.errmsg, $onResponse);
            rq.send();
            return true;
        };
        //store.fetchNewer();
    }
    
    /*void appendTodos(void* flatbuf)
    {
        auto wrapper = flatbuffers::GetRoot<todo::user::Todo_PList>(flatbuf);
        auto plist = wrapper->p();
        
        if (plist == nullptr || 0 == plist->size())
            return;
        
        auto slot = list_.at(0);
        
        // lock for ui updates
        nana::internal_scope_guard lock;
        
        // 1-column inline widgets
        if (!initialized)
        {
            item_offset = todo_items.size();
            
            for (int i = 0; i < PAGE_SIZE; ++i)
                slot.append({ "" });
            
            place.field_visible("list_", true);
            initialized = true;
        }
        
        //slot.at(i).text(0, "gg");
        int i = 0, len = std::min(PAGE_SIZE, static_cast<int>(plist->size()));
        for (; i < len; i++) todo_items[item_offset + i]->update(plist->Get(i));
        for (; i < PAGE_SIZE; i++) todo_items[item_offset + i]->update(nullptr);
        
        // 2-column text-only
        //for (int i = 0, len = plist->size(); i < len; i++)
        //    slot.append(plist->Get(i));
    }*/
};
