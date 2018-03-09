#include <forward_list>
#include <queue>
#include <thread>

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>

#include <coreds/rpc.h>
#include <coreds/pstore.h>

#include "util.h"
#include "ui.h"
#include "pager.h"
#include "app.h"

#include "../g/user/fbs_schema.h"

#include "user/index.h"
#include "exp.h"

namespace rpc = coreds::rpc;

struct About : ui::Panel
{
    nana::label text_{ *this, "about" };
    
    About(ui::Panel& owner, const char* field, const bool display = true) : ui::Panel(owner, 
        "vert"
        "<text_ weight=25>"
    )
    {
        place["text_"] << text_;
        
        place.collocate();
        
        owner.place[field] << *this;
        if (!display)
            owner.place.field_display(field, false);
    }
};

static const char* LINKS[] = {
    "<color=0x0080FF size=12 target=\"content_0\">    Home    </>",
    "<color=0x0080FF size=12 target=\"content_1\">    Todos    </>",
    "<color=0x0080FF size=12 target=\"content_2\">    About    </>"
};

static const int IDLE_INTERVAL = 10000,
        RECONNECT_INTERVAL = 5000;

struct App : rpc::Base
{
    util::RequestQueue rq;
    std::string buf;
    std::function<void()> $send{
        std::bind(&App::send, this)
    };
    
    ui::Form fm{ {273, 0, unsigned(util::WIDTH), unsigned(util::HEIGHT)}, 0xFFFFFF };
    
    ui::Place place{ fm, 
        "vert margin=5"
        "<header_ weight=20 margin=[0,30%]>"
        "<content_ margin=[5,0]>"
        "<footer_ weight=20>"
    };
    
    nana::label footer_{ fm, "Copyright 2018 <color=0x0080FF>David Yu</>" };
    
    ui::Panel content_{ fm,
        "vert"
        "<content_0>"
        "<content_1>"
        "<content_2>"
    };
    Home home{ content_, "content_0" };
    todo::user::Index todos{ content_, "content_1", false };
    About about{ content_, "content_2", false };
    
    std::forward_list<nana::label> links;
    std::vector<nana::label*> link_array;
    std::string current_target{ "content_0" };
    int current_selected{ 0 };
    
    brynet::net::EventLoop::PTR loop{ nullptr };
    brynet::net::HttpSession::PTR session{ nullptr };
    
    //int disconnect_count{ 0 };
    
    App(const rpc::Config config, const char* title) : rpc::Base(config)
    {
        fm.caption(title ? title : "Todo App");
        rq.send = [this]() {
            if (session != nullptr)
                loop->pushAsyncProc($send);
        };
    }

private:
    void links$$(const std::string& target)
    {
        int selected = std::atoi(target.c_str() + 8); // exclude the prefix: content_
        if (selected == current_selected)
            return;
        
        link_array[current_selected]->bgcolor(nana::colors::white);
        link_array[selected]->bgcolor(nana::color_rgb(0xF3F3F3));
        
        // hide current
        content_.place.field_display(current_target.c_str(), false);
        
        // set current
        current_selected = selected;
        current_target.assign(target);
        
        // show
        content_.place.field_display(target.c_str(), true);
        content_.place.collocate();
    }
    
    void onHttpData(const brynet::net::HTTPParser& httpParser,
            const brynet::net::HttpSession::PTR& session) override
    {
        auto body = httpParser.getBody();
        auto& req = rq.queue.front();
        auto ok = rpc::parseJson(body, req.res_type, parser, *req.errmsg);
        req.cb(ok ? parser.builder_.GetBufferPointer() : nullptr);
        rq.queue.pop();
        if (!rq.queue.empty())
            send();
    }
    
    void onHttpOpen(const brynet::net::HttpSession::PTR& session) override
    {
        this->session = session;
        
        if (!rq.queue.empty())
        {
            send();
            return;
        }
        
        if (!home.fetched_initial)
            home.store.fetchNewer();
        
        if (!todos.pager_.fetched_initial)
            todos.pager_.store.fetchNewer();
    }
    
    void onHttpClose(const brynet::net::HttpSession::PTR& session) override
    {
        this->session = nullptr;
        fd = SOCKET_ERROR;
        //connect(true);
    }
    
    void onLoop(const brynet::net::EventLoop::PTR& loop) override
    {
        if (this->loop == nullptr)
            this->loop = loop;
        
        if (isConnected())
        {
            // wait for epoll
            loop->loop(IDLE_INTERVAL);
        }
        else if (!connect())
        {
            {
                std::string msg;
                nana::internal_scope_guard lock;
                
                msg += "Could not connect to ";
                msg += req_host;
                
                if (todos.visible())
                    todos.pager_.msg_.update(msg);
                else
                    home.show(msg);
            }
            
            loop->loop(RECONNECT_INTERVAL);
        }
        /*else
        {
            fprintf(stdout, "connected\n");
        }*/
    }
    void send()
    {
        if (session != nullptr)
        {
            auto& req = rq.queue.front();
            post(session, req.uri, req.body);
        }
    }
    
public:
    void show(coreds::Opts opts)
    {
        home.init(opts, rq);
        todos.pager_.init(opts, rq);
        
        // header
        auto listener = [this](nana::label::command cmd, const std::string& target) {
            if (nana::label::command::click == cmd)
                links$$(target);
        };
        
        for (auto text : LINKS)
        {
            links.emplace_front(fm.handle());
            link_array.push_back(&links.front());
            
            place["header_"] << links.front()
                .text_align(nana::align::center)
                .format(true)
                .add_format_listener(listener)
                .caption(text);
            
            links.front().bgcolor(nana::colors::white);
        }
        
        link_array[0]->bgcolor(nana::color_rgb(0xF3F3F3));
        
        /*
        nana::drawing dw(*link_array[0]);
        dw.draw([](nana::paint::graphics& graph) {
            graph.rectangle(false, nana::colors::gray_border);
        });
        */
        
        place["content_"] << content_;
        
        place["footer_"] << footer_.text_align(nana::align::center).format(true);
        
        place.collocate();
        fm.show();
        start();
    }
};

namespace todo {

int run(int argc, char* argv[], const char* title)
{
    const auto config = rpc::Config::parseFrom(
        argc > 1 ? argv[1] : nullptr,
        argc > 2 ? argv[2] : nullptr,
        5000 // if no args provided, 127.0.0.1:5000 is the endpoint
    );
    
    if (config.host == nullptr)
    {
        fprintf(stderr, "Invalid endpoint %s\n", argv[1]);
        return 1;
    }
    
    App app(config, title);
    
    if (!app.parser.Parse(todo_user_schema))
    {
        fprintf(stderr, "Could not load schema.\n");
        return 1;
    }
    
    todo_items.reserve(PAGE_SIZE);
    
    coreds::Opts opts;
    opts.pageSize = PAGE_SIZE;
    opts.multiplier = MULTIPLIER;
    
    app.show(opts);
    nana::exec();
    return 0;
}

} // todo


