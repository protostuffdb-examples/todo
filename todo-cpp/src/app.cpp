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

struct About : ui::Panel, coreds::HasState<bool>
{
    util::RequestQueue& rq;
    
    nana::label content_{ *this,
        "Keyboard shortcuts:\n\n"
        "    ctrl + space \t=\t refresh\n\n"
        "    shift + space \t=\t toggle sort order\n\n"
        "    ctrl + shift + space \t=\t show/hide update form\n\n"
        "    esc \t=\t hide msg/form (if shown)\n\n"
        "    arrow-left \t=\t go to previous page or load more content if at end\n\n"
        "    arrow-right \t=\t go to next page or load more content if at end\n\n"
        "    ctrl + arrow-left \t=\t go to first page\n\n"
        "    ctrl + arrow-right \t=\t go to last page\n\n"
        "    ctrl + arrow-down \t=\t go to last item in the current page\n\n"
        "    ctrl + arrow-up \t=\t go to first item in the current page\n\n"
        "\nCredits:\n\n"
        "    <color=0x0080FF>leveldb</> - backend storage engine from Google by Jeff Dean and Sanjay Ghemawhat\n\n"
        "    <color=0x0080FF>uWebSockets</> - backend http/websocket engine by Alex Hultman @alexhultman\n\n"
        "    <color=0x0080FF>flatbuffers</> - serialization with json support by Wouter van Oortmerssen @aardappel\n\n"
        "    <color=0x0080FF>brynet</> - frontend http/websocket engine by @IronsDu\n\n"
        "    <color=0x0080FF>nana</> - frontend gui tookit by @cnjinhao\n\n"
    };
    
    ui::w$::Label text8_{ *this, nullptr, "about | 08 | g | simon", ui::fonts::r8 };
    ui::w$::Label text9_{ *this, nullptr, "about | 09 | g | says",  ui::fonts::r9 };
    ui::w$::Label text10_{ *this, nullptr, "about | 10 | g | the",  ui::fonts::r10 };
    ui::w$::Label text11_{ *this, nullptr, "about | 11 | g | quick",  ui::fonts::r11 };
    ui::w$::Label text12_{ *this, nullptr, "about | 12 | g | brown",  ui::fonts::r12 };
    ui::w$::Label text14_{ *this, nullptr, "about | 14 | g | fox",  ui::fonts::r14 };
    ui::w$::Label text16_{ *this, nullptr, "about | 16 | g | jumps",  ui::fonts::r16 };
    ui::w$::Label text18_{ *this, nullptr, "about | 18 | g | over",  ui::fonts::r18 };
    ui::w$::Label text20_{ *this, nullptr, "about | 20 | g | the",  ui::fonts::r20 };
    ui::w$::Label text22_{ *this, nullptr, "about | 22 | g | lazy",  ui::fonts::r22 };
    ui::w$::Label text24_{ *this, nullptr, "about | 24 | g | dog",  ui::fonts::r24 };
    
    About(ui::Panel& owner, 
            util::RequestQueue& rq,
            std::vector<coreds::HasState<bool>*>& container,
            const char* field, bool active = false) : ui::Panel(owner,
        "<content_>|40%"
        "<vert"
          "<text8_ weight=16>"
          "<weight=5>"
          "<text9_ weight=18>"
          "<weight=5>"
          "<text10_ weight=22>"
          "<weight=5>"
          "<text11_ weight=24>"
          "<weight=5>"
          "<text12_ weight=27>"
          "<weight=5>"
          "<text14_ weight=32>"
          "<weight=5>"
          "<text16_ weight=36>"
          "<weight=5>"
          "<text18_ weight=41>"
          "<weight=5>"
          "<text20_ weight=46>"
          "<weight=5>"
          "<text22_ weight=51>"
          "<weight=5>"
          "<text24_ weight=56>"
          "<weight=15>"
        ">"
        
    ), rq(rq)
    {
        container.push_back(this);
        
        place["content_"] << content_.format(true);
        styles::apply_default(content_);
        
        place["text8_"] << text8_;
        text8_.bg(colors::primary).fgcolor(nana::colors::white);
        
        place["text9_"] << text9_;
        text9_.bg(colors::primary).fgcolor(nana::colors::white);
        
        place["text10_"] << text10_;
        text10_.bg(colors::primary).fgcolor(nana::colors::white);
        
        place["text11_"] << text11_;
        text11_.bg(colors::primary).fgcolor(nana::colors::white);
        
        place["text12_"] << text12_;
        text12_.bg(colors::primary).fgcolor(nana::colors::white);
        
        place["text14_"] << text14_;
        text14_.bg(colors::primary).fgcolor(nana::colors::white);
        
        place["text16_"] << text16_;
        text16_.bg(colors::primary).fgcolor(nana::colors::white);
        
        place["text18_"] << text18_;
        text18_.bg(colors::primary).fgcolor(nana::colors::white);
        
        place["text20_"] << text20_;
        text20_.bg(colors::primary).fgcolor(nana::colors::white);
        
        place["text22_"] << text22_;
        text22_.bg(colors::primary).fgcolor(nana::colors::white);
        
        place["text24_"] << text24_;
        text24_.bg(colors::primary).fgcolor(nana::colors::white);
        
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
    "<target=\"content_0\">    Home    </>",
    "<target=\"content_1\">    Active    </>",
    "<target=\"content_2\">    Completed    </>",
    "<target=\"content_3\">    Compact    </>",
    "<target=\"content_4\">    About    </>"
};

struct App : rpc::Base
{
    ui::RootForm fm{ {util::LEFT, 0, unsigned(util::WIDTH), unsigned(util::sc->height)}, 0 | ui::WindowFlags::STATIC };
    
    ui::Place place{ fm, 
        "vert margin=5"
        "<header_ weight=32>"
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
    std::vector<coreds::HasState<bool>*> content_array;
    todo::user::Index home_{ content_, rq, content_array, "content_0", true };
    todo::user::Index active_{ content_, rq, content_array, "content_1" };
    todo::user::Index completed_{ content_, rq, content_array, "content_2" };
    todo::exp::Home exp_{ content_, rq, content_array, "content_3" };
    About about_{ content_, rq, content_array, "content_4" };
    
    std::vector<ui::w$::Label*> link_array;
    std::forward_list<ui::w$::Label> links;
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
        
        link_array[current_selected]->bg(nana::colors::white);
        link_array[selected]->bg(colors::lgray_darken);
        
        // hide current
        content_.place.field_display(current_target.c_str(), false);
        content_array[current_selected]->update(false);
        
        // set current
        current_selected = selected;
        current_target.assign(target);
        
        // prepare
        content_array[selected]->update(true);
        
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
            link_array[current_selected]->bg(colors::lgray_darken);
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
            links.emplace_front(fm, nullptr, "", ui::fonts::r14);
            
            auto& front = links.front();
            link_array.push_back(&front);
            
            front.$
                .text_align(nana::align::center)
                .format(true)
                .add_format_listener(listener)
                .caption(text);
            
            front.bg(nana::colors::white).fgcolor(colors::primary);
            
            place["header_"] << front;
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
    
    util::ScreenConfig sc;
    util::sc = &sc;
    
    #ifndef WIN32
    nana::API::window_icon_default(nana::paint::image("assets/icon.ico"));
    #endif
    
    App app(config, title);
    
    if (!app.parser.Parse(todo_user_schema))
    {
        fprintf(stderr, "Could not load schema.\n");
        return 1;
    }
    
    coreds::Opts opts;
    opts.pageSize = util::PAGE_SIZE;
    opts.multiplier = util::MULTIPLIER;
    
    app.show(opts);
    nana::exec();
    return 0;
}

} // todo


