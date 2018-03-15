#include <forward_list>
#include <queue>
#include <thread>

#include <coreds/nana/ui.h>
#include <coreds/nana/pager.h>

#include <coreds/rpc.h>
#include <coreds/pstore.h>

#include "user/index.h"

#include "exp.h"

#include "util.h"

#include "../g/user/fbs_schema.h"
#include "app.h"

namespace rpc = coreds::rpc;

struct About : ui::Panel, util::HasState<bool>
{
    util::RequestQueue& rq;
    nana::label text_{ *this, "about" };
    
    About(ui::Panel& owner, 
            util::RequestQueue& rq,
            std::vector<util::HasState<bool>*>& container,
            const char* field, bool active = false) : ui::Panel(owner, 
        "vert"
        "<text_ weight=25>"
    ), rq(rq)
    {
        container.push_back(this);
        
        place["text_"] << text_;
        
        place.collocate();
        
        owner.place[field] << *this;
        owner.place.field_display(field, active);
    }
    void update(bool on) override
    {
        
    }
};

static const char URI_TODO_COMPLETED[] = "/todo/user/qTodo0Completed";

static const int IDLE_INTERVAL = 10000,
        RECONNECT_INTERVAL = 5000;

static const char* LINKS[] = {
    "<color=0x0080FF size=12 target=\"content_0\">    Home    </>",
    "<color=0x0080FF size=12 target=\"content_1\">    Active    </>",
    "<color=0x0080FF size=12 target=\"content_2\">    Completed    </>",
    "<color=0x0080FF size=12 target=\"content_3\">    Exp    </>",
    "<color=0x0080FF size=12 target=\"content_4\">    About    </>"
};

struct App : rpc::Base
{
    ui::Form fm{ {273, 0, unsigned(util::WIDTH), unsigned(util::HEIGHT)}, 0 | ui::WindowFlags::STATIC };
    
    ui::Place place{ fm, 
        "vert margin=5"
        #ifdef WIN32
        "<header_ weight=35>"
        #else
        "<header_ weight=20>"
        #endif
        "<content_ margin=[5,0]>"
        "<footer_ weight=20>"
    };
    
    nana::label footer_{ fm, "Copyright 2018 <color=0x0080FF>David Yu</>" };
    
    ui::Panel content_{ fm,
        "vert"
        "<content_0>"
        "<content_1>"
        "<content_2>"
        "<content_3>"
        "<content_4>"
    };
    util::RequestQueue rq;
    std::vector<util::HasState<bool>*> content_array;
    todo::user::Index home_{ content_, rq, content_array, "content_0", true };
    todo::user::Index active_{ content_, rq, content_array, "content_1" };
    todo::user::Index completed_{ content_, rq, content_array, "content_2" };
    todo::exp::Home exp_{ content_, rq, content_array, "content_3" };
    About about_{ content_, rq, content_array, "content_4" };
    
    std::vector<nana::label*> link_array;
    std::forward_list<nana::label> links;
    std::string current_target{ "content_0" };
    int current_selected{ 0 };
    
    brynet::net::EventLoop::PTR loop{ nullptr };
    brynet::net::HttpSession::PTR session{ nullptr };
    
    std::function<void()> $send{
        std::bind(&App::send, this)
    };
    
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
        content_array[current_selected]->update(false);
        
        // set current
        current_selected = selected;
        current_target.assign(target);
        
        // show
        content_.place.field_display(target.c_str(), true);
        content_.place.collocate();
        content_array[selected]->update(true);
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
            send();
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
        {
            this->loop = loop;
            
            nana::internal_scope_guard lock;
            link_array[current_selected]->bgcolor(nana::color_rgb(0xF3F3F3));
            content_array[current_selected]->update(true);
        }
        
        if (isConnected())
        {
            // wait for epoll
            loop->loop(IDLE_INTERVAL);
        }
        else if (!connect())
        {
            if (home_.visible())
            {
                std::string msg;
                nana::internal_scope_guard lock;
                
                msg += "Could not connect to ";
                msg += req_host;
                
                home_.update(msg);
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
        home_.init(opts);
        active_.init(opts, [](std::string& buf, coreds::ParamRangeKey& prk) {
            buf += R"({"1":0,"4":)";
            prk.stringifyTo(buf);
            buf += '}';
            return URI_TODO_COMPLETED;
        });
        completed_.init(opts, [](std::string& buf, coreds::ParamRangeKey& prk) {
            buf += R"({"1":1,"4":)";
            prk.stringifyTo(buf);
            buf += '}';
            return URI_TODO_COMPLETED;
        });
        exp_.init(opts);
        
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
    
    coreds::Opts opts;
    opts.pageSize = util::PAGE_SIZE;
    opts.multiplier = util::MULTIPLIER;
    
    //nana::API::window_icon_default(nana::paint::image("assets/icon.png"));
    app.show(opts);
    nana::exec();
    return 0;
}

} // todo


