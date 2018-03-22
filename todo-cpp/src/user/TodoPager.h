#pragma once

#include <coreds/nana/pager.h>

#include "../assets.h"

#include "TodoForms.h"

namespace todo {
namespace user {

struct TodoItem;

struct TodoPager : ui::Pager<todo::Todo, todo::user::Todo, TodoItem>
{
    util::RequestQueue* rq{ nullptr };
    std::vector<coreds::HasState<int>*> state_items;
    
    ui::MsgPanel msg_ { *this, ui::MsgColors::DEFAULT };
    TodoUpdate fupdate_{ store, "Update Todo" };
private:
    TodoNew fnew_{ store, "New Todo" };
    
    //ui::w$::Input search_{ *this, nullptr, "Todo", fonts::md(), &colors::primary };
    ui::Icon add_{ *this, icons::plus, true };

    ui::ToggleIcon sort_{ *this, icons::arrow_down, icons::arrow_up };
    
    ui::Icon refresh_{ *this, icons::cw, true };
    
    nana::label page_info_{ *this, "" };
    std::string page_str;
    
    ui::Icon goto_first_{ *this, icons::angle_double_left, true };
    ui::Icon goto_left_{ *this, icons::angle_left, true };
    ui::Icon goto_right_{ *this, icons::angle_right, true };
    ui::Icon goto_last_{ *this, icons::angle_double_right, true };
    
    std::function<void(void* res)> $fetch$${
        std::bind(&TodoPager::fetch$$, this, std::placeholders::_1)
    };
    std::function<bool(coreds::ParamRangeKey prk)> $fetch{
        std::bind(&TodoPager::fetch, this, std::placeholders::_1)
    };
    
    std::function<void()> $fnewFocus{
        std::bind(&TodoPager::fnewFocus, this)
    };
    
    std::function<const char*(std::string& buf, coreds::ParamRangeKey& prk)> $filter{ nullptr };
public:
    TodoPager(nana::widget& owner) : ui::Pager<todo::Todo, todo::user::Todo, TodoItem>(owner,
        "vert margin=[5,0]"
        "<weight=25"
          //"<search_ weight=200>"
          //"<weight=15>"
          "<msg_>"
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
        "<items_ vert>"
    )
    {
        if (util::sc->hd)
        {
            page_info_.typeface(ui::fonts::r11);
        }
        
        //place["search_"] << search_;
        
        place["page_info_"] << page_info_
                .text_align(nana::align::center);
        
        place["msg_"] << msg_;
        
        place["sort_"] << sort_;
        sort_.on_.events().click(store.$toggleSort);
        sort_.off_.events().click(store.$toggleSort);
        
        place["refresh_"] << refresh_;
        refresh_.events().click(store.$refresh);
        
        place["goto_first_"] << goto_first_;
        goto_first_.events().click(store.$gotoFirst);
        
        place["goto_left_"] << goto_left_;
        goto_left_.events().click(store.$prevOrLoad);
        
        place["goto_right_"] << goto_right_;
        goto_right_.events().click(store.$nextOrLoad);
        
        place["goto_last_"] << goto_last_;
        goto_last_.events().click(store.$gotoLast);
    }
    void select(int idx) override
    {
        int prevIdx = getSelectedIdx();
        if (trySelect(idx))
        {
            store.select(idx);
            
            if (prevIdx != -1)
                state_items[prevIdx]->update(0);
            
            if (idx != -1)
                state_items[idx]->update(1);
        }
    }
protected:
    void selectForUpdate(int idx) override
    {
        if (idx != -1)
            state_items[idx]->update(-1);
    }
private:
    void fnewFocus()
    {
        fnew_.popTo(add_);
    }
    void beforePopulate() override
    {
        ui::visible(*this, false);
    }
    void afterPopulate(int selectedIdx) override
    {
        select(selectedIdx);
        
        page_str.clear();
        store.appendPageInfoTo(page_str);
        page_info_.caption(page_str);
        
        ui::visible(*this, true);
    }
    void afterPopulate()
    {
        afterPopulate(store.getSelectedIdx());
    }
    void fetch$$(void* res)
    {
        if (res == nullptr)
        {
            nana::internal_scope_guard lock;
            store.cbFetchFailed();
            msg_.update(store.errmsg);
        }
        else
        {
            store.cbFetchSuccess(flatbuffers::GetRoot<todo::user::Todo_PList>(res)->p());
        }
    }
    bool fetch(coreds::ParamRangeKey prk)
    {
        std::string buf;
        if ($filter)
        {
            rq->queue.emplace($filter(buf, prk), buf, "Todo_PList", &store.errmsg, $fetch$$);
        }
        else
        {
            prk.stringifyTo(buf);
            rq->queue.emplace("/todo/user/Todo/list", buf, "Todo_PList", &store.errmsg, $fetch$$);
        }
        
        rq->send();
        return true;
    }
public:
    void init(coreds::Opts opts,
            util::RequestQueue& rq,
            std::function<const char*(std::string& buf, coreds::ParamRangeKey& prk)> filter = nullptr)
    {
        this->rq = &rq;
        fnew_.rq = &rq;
        fupdate_.rq = &rq;
        
        if (filter)
        {
            $filter = filter;
        }
        else
        {
            place["add_"] << add_;
            add_.events().click($fnewFocus);
        }
        
        store.init(opts);
        store.$fnFetch = $fetch;
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
                    }
                    break;
                case coreds::EventType::VISIBLE:
                {
                    nana::internal_scope_guard lock;
                    if (on)
                        select(store.getSelectedIdx());
                    
                    page_str.clear();
                    store.appendPageInfoTo(page_str);
                    page_info_.caption(page_str);
                    
                    ui::visible(*this, on);
                    break;
                }
            }
        };
        
        hide();
        collocate(opts.pageSize);
        store.populate();
        show();
    }
};

struct TodoItem : ui::BgPanel, coreds::HasState<int>
{
private:
    TodoPager& pager;
    const int idx;
    ui::Icon pencil_{ *this, icons::pencil, true };
    nana::label title_{ *this, "" };
    nana::label ts_{ *this, "" };
    ui::ToggleIcon completed_{ *this, icons::circle, icons::circle_empty };
    
    todo::Todo* pojo{ nullptr };
    
    std::function<void(void* res)> $toggleCompleted$${
        std::bind(&TodoItem::toggleCompleted$$, this, std::placeholders::_1)
    };
    std::function<void()> $toggleCompleted{
        std::bind(&TodoItem::toggleCompleted, this)
    };
public:
    TodoItem(nana::widget& owner) : ui::BgPanel(owner,
        "margin=[5,10,5,5]"
        "<pencil_ weight=16 margin=[2,0,0,0]>"
        "<weight=5>"
        "<title_ margin=[2,0,0,0]>"
        "<ts_ weight=150 margin=[2,0,0,0]>"
        "<weight=10>"
        "<completed_ weight=16 margin=[2,0,0,0]>"
        ),
        pager(static_cast<TodoPager&>(owner)),
        idx(pager.size())
    {
        pager.state_items.push_back(this);
        
        auto $select = [this]() {
            pager.select(idx);
        };
        
        auto $show_form = [this]() {
            popForm();
        };
        
        place["pencil_"] << pencil_;
        pencil_.events().click($show_form);
        
        place["title_"] << title_
            .text_align(nana::align::left)
            .transparent(true);
        title_.typeface(fonts::sm());
        title_.events().click($select);
        title_.events().key_press(pager.$navigate);
        
        place["ts_"] << ts_
            .text_align(nana::align::right)
            .transparent(true);
        ts_.typeface(fonts::sm());
        ts_.events().click($select);
        ts_.events().key_press(pager.$navigate);
        
        place["completed_"] << completed_;
        completed_.on_.events().click($toggleCompleted);
        completed_.off_.events().click($toggleCompleted);
        
        if (idx != 0)
        {
            nana::drawing dw(*this);
            dw.draw([](nana::paint::graphics& graph) {
                ui::border_top(graph, colors::border);
            });
            
            /*dw.draw([](nana::paint::graphics& graph) {
                graph.frame_rectangle(nana::rectangle(graph.size()),
                    nana::colors::white, // left
                    colors::border, // top
                    nana::colors::white, // right
                    nana::colors::white); // bottom
            });*/
        }
        
        place.collocate();
        ui::visible(pencil_, false);
        //hide();
    }
private:
    void popForm()
    {
        pager.fupdate_.popTo(*this, pojo, this, 5);
    }
    void toggleCompleted$$(void* res)
    {
        nana::internal_scope_guard lock;
        
        pager.store.loading(false);
        
        if (res == nullptr)
        {
            pager.msg_.update(pager.store.errmsg);
        }
        else
        {
            completed_.update((pojo->completed = !pojo->completed));
        }
    }
    void toggleCompleted()
    {
        if (pager.store.loading())
            return;
        
        std::string buf;
        util::appendUpdateReqTo(buf, pojo->key.c_str(), 4, !pojo->completed);
        
        pager.rq->queue.emplace("/todo/user/Todo/update", buf, nullptr, &pager.store.errmsg, $toggleCompleted$$);
        pager.rq->send();
        
        pager.store.loading(true);
    }
    void updateTimeago(int64_t ts)
    {
        std::string timeago;
        timeago.reserve(16); // just moments ago
        coreds::util::appendTimeagoTo(timeago, pojo->ts, ts);
        ts_.caption(timeago);
    }
public:
    void update(todo::Todo* message, int64_t ts)
    {
        pojo = message;
        if (message == nullptr)
        {
            hide();
            return;
        }
        
        title_.caption(pojo->title);
        
        updateTimeago(ts);
        
        completed_.update(pojo->completed);
        
        show();
    }
    void update(int field) override
    {
        switch (field)
        {
            case -2: // called after close
                title_.focus();
                break;
            case -1: // select for update
                popForm();
                break;
            case 0:
                ui::visible(pencil_, false);
                break;
            case 1:
                ui::visible(pencil_, true);
                break;
            case 2:
                updateTimeago(coreds::util::now());
                break;
            case 3:
                title_.caption(pojo->title);
                break;
            case 4:
                completed_.update(pojo->completed);
                break;
        }
    }
};

} // user
} // todo
