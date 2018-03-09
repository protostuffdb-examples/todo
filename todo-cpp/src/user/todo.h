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
    bool fetched_initial{ false };
    
    ui::MsgPanel msg_ { *this, ui::MsgColors::DEFAULT };
private:
    ui::ToggleIcon sort_{ *this, "assets/png/arrow-down.png", "assets/png/arrow-up.png" };
    
    nana::label page_info_{ *this, "" };
    std::string page_str;
    
    std::function<void(void* res)> $onResponse{
        std::bind(&TodoPager::onResponse, this, std::placeholders::_1)
    };
    
public:
    TodoPager(nana::widget& owner) : ui::Pager<Todo, todo::user::Todo, TodoItemPanel>(owner,
        "vert margin=[5,0]"
        "<weight=40"
          "<sort_ weight=16>"
          "<msg_>"
          "<page_info_ weight=160>"
        ">"
        "<items_ vert>"
    )
    {
        auto $sort = [this]() {
            store.toggleDesc();
        };
        
        sort_.on_.events().click($sort);
        sort_.off_.events().click($sort);
        place["sort_"] << sort_;
        
        place["msg_"] << msg_;
        
        place["page_info_"] << page_info_
                .text_align(nana::align::right);
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
            store.cbFetchFailed();
        }
        else
        {
            store.cbFetchSuccess(flatbuffers::GetRoot<todo::user::Todo_PList>(res)->p());
            fetched_initial = true;
        }
    }
    void init(coreds::Opts opts, util::RequestQueue& rq)
    {
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
    
    Todo* pojo{ nullptr };
    
    TodoItemPanel(nana::widget& owner) : ui::BgPanel(owner,
        "margin=[5,10]"
        "<title_>"
        "<ts_>"
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
        
        place.collocate();
        hide();
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
        
        show();
    }
};

} // todo
