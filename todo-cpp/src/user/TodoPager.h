#pragma once

#include <coreds/nana/pager.h>

#include "../icons.h"

#include "TodoForms.h"

namespace todo {
namespace user {

struct TodoItem;

struct TodoPager : ui::Pager<todo::Todo, todo::user::Todo, TodoItem>
{
    util::RequestQueue* rq{ nullptr };
    ui::MsgPanel msg_ { *this, ui::MsgColors::DEFAULT };
private:
    ui::Icon add_{ *this, icons::plus, true };

    ui::ToggleIcon sort_{ *this, icons::arrow_down, icons::arrow_up };
    
    ui::Icon refresh_{ *this, icons::cw, true };
    
    nana::label page_info_{ *this, "" };
    std::string page_str;
    
    ui::Icon goto_first_{ *this, icons::angle_double_left, true };
    ui::Icon goto_left_{ *this, icons::angle_left, true };
    ui::Icon goto_right_{ *this, icons::angle_right, true };
    ui::Icon goto_last_{ *this, icons::angle_double_right, true };
    
    TodoNew fnew_{ store, "New Todo" };
    
    std::function<void(void* res)> $fetch$${
        std::bind(&TodoPager::fetch$$, this, std::placeholders::_1)
    };
    std::function<bool(coreds::ParamRangeKey prk)> $fetch{
        std::bind(&TodoPager::fetch, this, std::placeholders::_1)
    };
public:
    TodoPager(nana::widget& owner) : ui::Pager<todo::Todo, todo::user::Todo, TodoItem>(owner,
        "vert margin=[5,0]"
        "<weight=25"
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
        auto $sort = [this]() { store.toggleDesc(); };
        auto $refresh = [this]() { store.fetchUpdate(); };
        auto $first = [this]() { store.pageTo(0); };
        auto $last = [this]() { store.pageTo(store.getPageCount()); };
        auto $left = [this]() {
            if (0 != store.getPage())
                store.pageTo(store.getPage() - 1);
            else
                store.fetchNewer();
        };
        auto $right = [this]() {
            if (store.getPageCount() != store.getPage())
                store.pageTo(store.getPage() + 1);
            else
                store.fetchOlder();
        };
        auto $add = [this]() {
            fnew_.popTo(add_, 50);
            fnew_.focus();
        };
        
        place["page_info_"] << page_info_
                .text_align(nana::align::center);
        
        place["msg_"] << msg_;
        
        place["add_"] << add_;
        add_.events().click($add);
        
        place["sort_"] << sort_;
        sort_.on_.events().click($sort);
        sort_.off_.events().click($sort);
        
        place["refresh_"] << refresh_;
        refresh_.events().click($refresh);
        
        place["goto_first_"] << goto_first_;
        goto_first_.events().click($first);
        
        place["goto_left_"] << goto_left_;
        goto_left_.events().click($left);
        
        place["goto_right_"] << goto_right_;
        goto_right_.events().click($right);
        
        place["goto_last_"] << goto_last_;
        goto_last_.events().click($last);
    }
private:
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
        prk.stringifyTo(buf);
        
        rq->queue.emplace("/todo/user/Todo/list", buf, "Todo_PList", &store.errmsg, $fetch$$);
        rq->send();
        return true;
    }
public:
    void init(coreds::Opts opts, util::RequestQueue& requestQueue)
    {
        rq = &requestQueue;
        fnew_.rq = &requestQueue;
        
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
        
        collocate(opts.pageSize);
    }
};

struct TodoItem : ui::BgPanel
{
private:
    TodoPager& pager;
    const int idx;
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
        "margin=[5,10]"
        "<title_>"
        "<ts_ weight=130>"
        "<weight=10>"
        "<completed_ weight=16>"
        ),
        pager(static_cast<TodoPager&>(owner)),
        idx(pager.size())
    {
        auto $selected = [this]() {
            pager.select(idx);
        };
        
        place["title_"] << title_
            .text_align(nana::align::left)
            .transparent(true);
        title_.events().click($selected);
        title_.events().key_press(pager.$navigate);
        
        place["ts_"] << ts_
            .text_align(nana::align::right)
            .transparent(true);
        ts_.events().click($selected);
        ts_.events().key_press(pager.$navigate);
        
        place["completed_"] << completed_;
        completed_.on_.events().click($toggleCompleted);
        completed_.off_.events().click($toggleCompleted);
        
        place.collocate();
        hide();
    }
private:
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
public:
    void update(todo::Todo* message)
    {
        pojo = message;
        if (message == nullptr)
        {
            hide();
            return;
        }
        
        title_.caption(pojo->title);
        
        std::string timeago;
        timeago.reserve(16); // just moments ago
        coreds::util::appendTimeagoTo(timeago, pojo->ts);
        ts_.caption(timeago);
        
        completed_.update(pojo->completed);
        
        show();
    }
};

} // user
} // todo