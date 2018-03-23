#pragma once

#include <coreds/nana/ui.h>
//#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
//#include <nana/gui/widgets/textbox.hpp>

#include "assets.h"

#include "user/TodoForms.h"

namespace todo {
namespace exp {

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
        // listbox inner
        LB_FIELDS = 2,
        LB_INNER = util::MARGIN * 3 * LB_FIELDS,
        COMPLETED_WIDTH = 20,
        TITLE_WIDTH = util::LB_WIDTH - LB_INNER - COMPLETED_WIDTH;

// hack
struct TodoItem;
static std::vector<TodoItem*> todo_items;

struct TodoItem : nana::listbox::inline_notifier_interface
{
    std::function<void()> $selected{ std::bind(&TodoItem::selected, this) };
    
    inline_indicator* ind_ { nullptr };
    index_type pos_;
    
    ui::DeferredPanel pnl_ {
        "margin=[1,10]"
        "<title_ margin=[2,10,0,0]>"
        "<ts_ weight=130 margin=[2,10,0,0]>"
        "<completed_ weight=16 margin=[3,0,0,0]>"
        //"<txt_ weight=200 margin=[0,5,0,0]>"
        //"<btn_ weight=25>" 
    };
    nana::label title_;
    nana::label ts_;
    ui::DeferredToggleIcon completed_{ icons::square_checked, icons::square_empty };
    //nana::textbox txt_;
    //nana::button btn_;
    
    todo::Todo* pojo{ nullptr };
    todo::TodoStore* store{ nullptr };
    util::RequestQueue* rq{ nullptr };
    int idx;
    
    coreds::HasState<const std::string&>* hasMsg { nullptr };
    
    std::function<void(void* res)> $toggleCompleted$${
        std::bind(&TodoItem::toggleCompleted$$, this, std::placeholders::_1)
    };
    std::function<void()> $toggleCompleted{
        std::bind(&TodoItem::toggleCompleted, this)
    };
    
    TodoItem()
    {
        todo_items.push_back(this);
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
        title_.fgcolor(colors::text);
        pnl_.place["title_"] << title_;
        
        // ts
        ts_.create(pnl_);
        ts_.transparent(true)
            .text_align(nana::align::right)
            .events().click($selected);
        ts_.fgcolor(colors::text);
        pnl_.place["ts_"] << ts_;
        
        completed_.create(pnl_);
        pnl_.place["completed_"] << completed_;
        completed_.on_.events().click($toggleCompleted);
        completed_.off_.events().click($toggleCompleted);
        
        /*
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
        */
    }
    void toggleCompleted$$(void* res)
    {
        nana::internal_scope_guard lock;
        
        store->loading(false);
        
        if (res == nullptr)
        {
            hasMsg->update(store->errmsg);
        }
        else
        {
            completed_.update((pojo->completed = !pojo->completed));
        }
    }
    void toggleCompleted()
    {
        if (store->loading())
            return;
        
        std::string buf;
        util::appendUpdateReqTo(buf, pojo->key.c_str(), 4, !pojo->completed);
        
        rq->queue.emplace("/todo/user/Todo/update", buf, nullptr, &store->errmsg, $toggleCompleted$$);
        rq->send();
        
        store->loading(true);
    }
public:
    void init(int idx,
            todo::TodoStore& store,
            util::RequestQueue& rq,
            coreds::HasState<const std::string&>* hasMsg,
            std::function<void(const nana::arg_keyboard& arg)> $navigate)
    {
        this->idx = idx;
        this->store = &store;
        this->rq = &rq;
        this->hasMsg = hasMsg;
        
        title_.events().key_press($navigate);
        ts_.events().key_press($navigate);
    }
    void selected()
    {
        ind_->selected(pos_);
    }
    void update(todo::Todo* message, int64_t ts)
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
        coreds::util::appendTimeagoTo(timeago, pojo->ts, ts);
        ts_.caption(timeago);
        
        completed_.update(pojo->completed);
        
        pnl_.show();
    }
private:
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

struct Home : ui::Panel, coreds::HasState<bool>, coreds::HasState<const std::string&>
{
    todo::TodoStore store;
private:
    util::RequestQueue& rq;
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
    
    ui::Icon add_{ *this, icons::plus, true };
    
    ui::ToggleIcon sort_{ *this, icons::arrow_down, icons::arrow_up };
    
    ui::Icon refresh_{ *this, icons::cw, true };
    
    ui::BgPanel msg_panel_{ *this,
        "margin=[3,2,1,3]"
        "<msg_>"
        "<msg_close_ weight=20>",
        0xFFFFFF
    };
    nana::label msg_{ msg_panel_, "" };
    nana::label msg_close_{ msg_panel_, "<bold target=\"2\"> x </>" };
    
    nana::label page_info_{ *this, "" };
    
    ui::Icon goto_first_{ *this, icons::angle_double_left, true };
    ui::Icon goto_left_{ *this, icons::angle_left, true };
    ui::Icon goto_right_{ *this, icons::angle_right, true };
    ui::Icon goto_last_{ *this, icons::angle_double_right, true };
    
    todo::user::TodoNew fnew_{ store, "New Todo" };
    
    nana::listbox list_{ *this, { 0, 25 + util::MARGIN, unsigned(util::LB_WIDTH), unsigned(util::sc->lb_height - (25 + util::MARGIN)) } };
    
    std::string page_str;
    
    coreds::Opts opts;
    
    bool initialized{ false };
    
    int item_offset;
    
public:
    Home(ui::Panel& owner,
            util::RequestQueue& rq,
            std::vector<coreds::HasState<bool>*>& container,
            const char* field, bool active = false) : ui::Panel(owner,
        "vert margin=[5,0]"
        "<horizontal weight=25"
          "<search_ weight=200 margin=[0,15,0,1]>"
          "<msg_panel_>"
          "<weight=10>"
          "<add_ weight=20>"
          "<weight=15>"
          "<sort_ weight=20>"
          "<weight=15>"
          "<refresh_ weight=20>"
          "<page_info_ weight=160>"
          "<goto_first_ weight=20>"
          "<weight=10>"
          "<goto_left_ weight=20>"
          "<weight=10>"
          "<goto_right_ weight=20>"
          "<weight=10>"
          "<goto_last_ weight=20>"
        ">"
        "<list_>"
    ), rq(rq)
    {
        container.push_back(this);
        
        fnew_.rq = &rq;
        
        auto $add = [this]() {
            fnew_.popTo(add_);
        };
        
        if (util::sc->hd)
        {
            page_info_.typeface(ui::fonts::r11);
        }
        
        place["search_"] << search_
            .tip_string("Todo").borderless(true);
        search_.typeface(fonts::md());
        nana::API::effects_edge_nimbus(search_, nana::effects::edge_nimbus::none);
        
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
        
        place["add_"] << add_;
        add_.events().click($add);
        
        place["sort_"] << sort_;
        sort_.on_.events().click(store.$toggleSort);
        sort_.off_.events().click(store.$toggleSort);
        
        place["refresh_"] << refresh_;
        refresh_.events().click(store.$refresh);
        
        place["page_info_"] << page_info_
                .text_align(nana::align::center);
        
        place["goto_first_"] << goto_first_;
        goto_first_.events().click(store.$gotoFirst);
        
        place["goto_left_"] << goto_left_;
        goto_left_.events().click(store.$prevOrLoad);
        
        place["goto_right_"] << goto_right_;
        goto_right_.events().click(store.$nextOrLoad);
        
        place["goto_last_"] << goto_last_;
        goto_last_.events().click(store.$gotoLast);
        
        // =====================================
        
        // listbox
        list_.show_header(false);
        list_.enable_single(true, true);
        
        // 1-column inline widgets
        list_.append_header("", util::LB_PANEL_WIDTH);
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
        if (on && !store.loading())
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
    void update(const std::string& msg) override
    {
        if (msg.empty())
            msg_panel_.hide();
        else
            show(msg);
    }
    
private:
    void lazyInit()
    {
        item_offset = todo_items.size();
        
        int len = opts.pageSize;
        auto slot = list_.at(0);
        for (int i = 0; i < len; ++i)
            slot.append({ "" });
        
        // the stmt below creates the items
        place.field_visible("list_", true);
        place.field_visible("list_", false);
        
        for (int i = 0; i < len; ++i)
            todo_items[item_offset + i]->init(i, store, rq, this, $navigate);
    }
    void populate(int idx, todo::Todo* pojo, int64_t ts)
    {
        if (!initialized)
        {
            lazyInit();
            initialized = true;
        }
        todo_items[item_offset + idx]->update(pojo, ts);
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
            case 2:
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
    void init(coreds::Opts o)
    {
        opts = o;
        opts.pageSize += util::sc->hd ? 20 : 5;
        
        if (todo_items.empty())
            todo_items.reserve(opts.pageSize);
        
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
        store.$fnPopulate = [this](int idx, todo::Todo* pojo, int64_t ts) {
            populate(idx, pojo, ts);
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
                    sort_.update(on);
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
        store.$fnFetch = [this](coreds::ParamRangeKey prk) {
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

} // exp
} // todo
