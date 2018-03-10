#pragma once

#include "../../g/user/index_generated.h"

#include "../util.h"
#include "../pager.h"

namespace todo {

struct Todo// : brynet::NonCopyable
{
    std::string key;
    uint64_t ts;
    
    std::string title;
    bool completed;
    
    Todo(const todo::user::Todo* src):
        key(src->key()->str()),
        ts(src->ts()),
        title(src->title()->str()),
        completed(src->completed())
    {
        
    }
};

struct TodoItemPanel;

struct TodoPager : ui::Pager<Todo, todo::user::Todo, TodoItemPanel>
{
    util::RequestQueue* rq{ nullptr };
    ui::MsgPanel msg_ { *this, ui::MsgColors::DEFAULT };
private:
    ui::ToggleIcon sort_{ *this, "assets/png/arrow-down.png", "assets/png/arrow-up.png" };
    
    ui::Icon refresh_{ *this, "assets/png/cw.png", true };
    
    nana::label page_info_{ *this, "" };
    std::string page_str;
    
    ui::Icon goto_first_{ *this, "assets/png/angle-double-left.png", true };
    ui::Icon goto_left_{ *this, "assets/png/angle-left.png", true };
    ui::Icon goto_right_{ *this, "assets/png/angle-right.png", true };
    ui::Icon goto_last_{ *this, "assets/png/angle-double-right.png", true };
    
    std::function<void(void* res)> $onResponse{
        std::bind(&TodoPager::onResponse, this, std::placeholders::_1)
    };
public:
    TodoPager(nana::widget& owner) : ui::Pager<Todo, todo::user::Todo, TodoItemPanel>(owner,
        "vert margin=[5,0]"
        "<weight=25"
          "<msg_>"
          "<weight=10>"
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
        
        place["page_info_"] << page_info_
                .text_align(nana::align::center);
        
        place["msg_"] << msg_;
        
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
    void onResponse(void* res)
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
    void init(coreds::Opts opts, util::RequestQueue& rq)
    {
        this->rq = &rq;
        
        store.init(opts);
        store.$fnKey = [](const Todo& pojo) {
            return pojo.key.c_str();
        };
        store.$fnKeyFB = [](const todo::user::Todo* message) {
            return message->key()->c_str();
        };
        store.$fnUpdate = [](Todo& pojo, const todo::user::Todo* message) {
            message->title()->assign_to(pojo.title);
            pojo.completed = message->completed();
        };
        store.$fnPopulate = [this](int idx, Todo* pojo) {
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
        store.$fnFetch = [this, &rq](coreds::ParamRangeKey prk) {
            std::string buf;
            prk.stringifyTo(buf);
            
            rq.queue.emplace("/todo/user/Todo/list", buf, "Todo_PList", &store.errmsg, $onResponse);
            rq.send();
            return true;
        };
        
        collocate(opts.pageSize);
        
        //store.fetchNewer();
    }
};

struct TodoItemPanel : ui::BgPanel
{
    TodoPager& pager;
    const int idx;
    nana::label title_{ *this, "" };
    nana::label ts_{ *this, "" };
    ui::ToggleIcon completed_ { *this, "assets/png/circle.png", "assets/png/circle-empty.png" };
    
    Todo* pojo{ nullptr };
    
    std::function<void(void* res)> $onResponse{
        std::bind(&TodoItemPanel::onResponse, this, std::placeholders::_1)
    };
    std::function<void()> $toggleCompleted{
        std::bind(&TodoItemPanel::toggleCompleted, this)
    };
    
    TodoItemPanel(nana::widget& owner) : ui::BgPanel(owner,
        "margin=[5,10]"
        "<title_>"
        "<ts_ weight=100>"
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
    
    void toggleCompleted()
    {
        std::string buf;
        util::appendUpdateReqTo(buf, pojo->key.c_str(), 4, !pojo->completed);
        
        pager.rq->queue.emplace("/todo/user/Todo/update", buf, nullptr, &pager.store.errmsg, $onResponse);
        pager.rq->send();
    }
    
    void onResponse(void* res)
    {
        nana::internal_scope_guard lock;
        if (res == nullptr)
        {
            pager.msg_.update(pager.store.errmsg);
        }
        else
        {
            completed_.update((pojo->completed = !pojo->completed));
        }
    }
    
    void update(Todo* message)
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

} // todo
