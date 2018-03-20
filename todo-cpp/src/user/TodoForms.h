#pragma once

#include <coreds/nana/pager.h>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/textbox.hpp>

#include "../util.h"

#include "../todo.h"

namespace todo {
namespace user {

struct TodoNew : ui::SubForm
{
    const bool close_on_success;
    util::RequestQueue* rq{ nullptr };
private:
    todo::TodoStore& store;
    std::string errmsg;
    
    std::function<void(void* res)> $submit$${
        std::bind(&TodoNew::submit$$, this, std::placeholders::_1)
    };
    std::function<void(const nana::arg_keyboard& arg)> $key_press{
        std::bind(&TodoNew::key_press, this, std::placeholders::_1)
    };
    
    int flex_height{ 0 };
    
    ui::w$::Input title_{ *this, &flex_height, "Title *", fonts::lg(), &colors::border };
    ui::MsgPanel msg_{ *this, ui::MsgColors::DEFAULT };
    nana::button submit_{ *this, "Submit" };
    
    ui::Place place{ *this,
        "vert margin=10"
        "<title_>"
        "<weight=10>"
        "<submit_ weight=30>"
        "<weight=10>"
        "<msg_ weight=20>"
    };
public:
    TodoNew(TodoStore& store_, const char* title, bool close_on_success = false):
        ui::SubForm({0, 0, 360, 90}, title),
        close_on_success(close_on_success),
        store(store_)
    {
        resizeY(flex_height);
        
        place["title_"] << title_;
        title_.$.events().key_press($key_press);
        
        place["msg_"] << msg_;
        
        place["submit_"] << submit_;
        submit_.typeface(fonts::lg());
        submit_.events().click([this] {
            submit();
        });
        
        place.collocate();
        ui::visible(msg_, false);
    }
private:
    void submit$$(void* res)
    {
        nana::internal_scope_guard lock;
        
        store.loading(false);
        title_.$.editable(true);
        
        if (res == nullptr)
        {
            msg_.update(errmsg);
        }
        else
        {
            title_.$.caption("");
            store.prependAll(flatbuffers::GetRoot<todo::user::Todo_PList>(res)->p(), true);
            
            if (close_on_success)
                close();
            else
                title_.$.focus();
        }
    }
    void submit()
    {
        if (store.loading())
            return;
        
        auto title = title_.$.caption();
        if (title.empty())
        {
            msg_.update(msgs::validation_required);
            title_.$.focus();
            return;
        }
        
        auto lastSeen = store.front();
        std::string buf;
        util::appendCreateReqTo(buf, lastSeen == nullptr ? nullptr : lastSeen->key.c_str(), title);
        
        rq->queue.emplace("/todo/user/Todo/create", buf, "Todo_PList", &errmsg, $submit$$);
        rq->send();
        
        title_.$.editable(false);
        ui::visible(msg_, false);
        store.loading(true);
    }
    void key_press(const nana::arg_keyboard& arg)
    {
        switch (arg.key)
        {
            case nana::keyboard::enter:
                submit();
                break;
            case nana::keyboard::escape:
                if (msg_.visible())
                    ui::visible(msg_, false);
                else
                    close();
                break;
        }
    }
public:
    void focus()
    {
        title_.$.focus();
    }
};

} // user
} // todo
