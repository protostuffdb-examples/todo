#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/label.hpp>
#include <forward_list>
#include <thread>

#include "ui.h"
#include "rpc.h"

#include "../g/user/fbs_schema.h"
#include "../g/user/index_generated.h"

static void printTodos(void* flatbuf)
{
    auto wrapper = flatbuffers::GetRoot<todo::user::Todo_PList>(flatbuf);
    auto plist = wrapper->p();
    fprintf(stdout, "%d todo(s)\n", plist->Length());
    for (auto it = plist->begin(); it != plist->end(); ++it)
        fprintf(stdout, "  key: %s, title: %s\n", it->key()->c_str(), it->title()->c_str());
}

struct Home : ui::Panel
{
    nana::label left{ *this, "left" };
    nana::label right{ *this, "right" };
    
    Home(ui::Panel& owner, const char* field, const bool display = true) : ui::Panel(owner, 
        "<left_ weight=30%>"
        "<right_>"
    )
    {
        left.bgcolor(nana::color_rgb(0xFAFAFA));
        
        place["left_"] << left;
        place["right_"] << right;
        place.collocate();
        
        owner.place[field] << *this;
        if (!display)
            owner.place.field_display(field, false);
    }
};

static const char MALFORMED_MESSAGE[] = "Malformed message.";

static const char* LINKS[] = {
    "<color=0x0080FF size=11 target=\"content_0\">Home</>",
    "<color=0x0080FF size=11 target=\"content_1\">Test</>",
    "<color=0x0080FF size=11 target=\"content_2\">About</>"
};

struct App : rpc::Base
{
    std::forward_list<nana::label> links;
    int current_selected{ 0 };
    std::string current_target{ "content_0" };
    
    ui::Form fm{ {273, 0, 1005, 710}, "Layout example", 0xFFFFFF };
    nana::place place{ fm };
    nana::label bottom{ fm, "Copyright 2018 <color=0x0080FF>David Yu</>" };
    ui::Panel content{ fm,
        "vert"
        "<content_0>"
        "<content_1>"
        "<content_2>"
    };
    Home home{ content, "content_0" };
    nana::label test{ content, "test" };
    nana::label about{ content, "about" };
    
    bool fetched_initial{ false };
    
    App(const char* host, int port) : Base(host, port)
    {
        place.div(
            "vert margin=5"
            "<header_ weight=20 margin=[0,30%]>"
            "<content_>"
            "<footer_ weight=20>"
        );
        
        about.bgcolor(nana::color_rgb(0xFCFCFC));
        
        // content
        //content.place["content_0"] << home;
        
        content.place["content_1"] << test;
        content.place.field_display("content_1", false);
        
        content.place["content_2"] << about;
        content.place.field_display("content_2", false);
        
        content.place.collocate();
        place["content_"] << content;
        
        // bottom
        place["footer_"] << bottom.text_align(nana::align::center).format(true);
    }
    
    void links$$(const std::string& target)
    {
        int selected = target.back() - 48;
        if (selected == current_selected)
            return;
        
        // hide current
        content.place.field_display(current_target.c_str(), false);
        
        // set current
        current_selected = selected;
        current_target[current_target.size() - 1] = target.back();
        
        // show
        content.place.field_display(target.c_str(), true);
        content.place.collocate();
    }
    
    void onHttpData(const brynet::net::HTTPParser& httpParser,
            const brynet::net::HttpSession::PTR& session) override
    {
        auto body = httpParser.getBody();
        if (rpc::parseJson(body, "Todo_PList", parser, errmsg))
        {
            printTodos(parser.builder_.GetBufferPointer());
            fetched_initial = true;
        }
        else
        {
            // TODO show errmsg
            fprintf(stdout, "Error:\n%s\n", errmsg.c_str());
        }
    }
    
    void onHttpOpen(const brynet::net::HttpSession::PTR& session) override
    {
        if (!fetched_initial)
            post(session, "/todo/user/Todo/list", R"({"1":true,"2":31})");
    }
    
    void onHttpClose(const brynet::net::HttpSession::PTR& session) override
    {
        fd = SOCKET_ERROR;
        //connect(true);
    }
    
    void onLoop(const brynet::net::EventLoop::PTR& loop) override
    {
        if (isConnected())
        {
            // wait for epoll
            loop->loop(10000);
        }
        else if (!connect(false))
        {
            // TODO show error
            
            // reconnect every 5 seconds
            loop->loop(5000);
        }
        else
        {
            fprintf(stdout, "connected\n");
        }
    }
    
    int show()
    {
        // header
        auto listener = [this](nana::label::command cmd, const std::string& target) {
            if (nana::label::command::click == cmd)
                links$$(target);
        };
        
        for (auto text : LINKS)
        {
            links.emplace_front(fm.handle());
            auto& link = links.front();
            
            link.text_align(nana::align::center)
                .format(true)
                .add_format_listener(listener)
                .caption(text);
            
            place["header_"] << link;
        }
        
        place.collocate();
        fm.show();
        start();
        nana::exec();
        return 0;
    }
};

static const char DEFAULT_HOST[] = "127.0.0.1";
static const int DEFAULT_PORT = 5000;

static const char* resolveIpPort(char* arg, int* port)
{
    if (arg == nullptr)
    {
        *port = DEFAULT_PORT;
        return DEFAULT_HOST;
    }
    
    char* colon = strchr(arg, ':');
    if (colon == nullptr)
    {
        *port = DEFAULT_PORT;
        return arg;
    }
    
    if (':' == *arg)
    {
        *port = std::atoi(arg + 1);
        return DEFAULT_HOST;
    }
    
    *(colon++) = '\0';
    *port = std::atoi(colon);
    
    return arg;
}

int main(int argc, char* argv[])
{
    int port;
    const char* host = resolveIpPort(argc > 1 ? argv[1] : nullptr, &port);
    /*
    flatbuffers::Parser parser;
    if (!parser.Parse(todo_user_schema) || !parser.SetRootType("Todo_PList"))
    {
        fprintf(stderr, "Could not load schema.\n");
        return 1;
    }
    
    if (!parser.ParseJson(R"({"p":[{"key":"CgAAAAAAAACZ","ts":1491921868559,"title":"world","completed":false}]})"))
    {
        fprintf(stderr, "Failed to parse json.\n%s\n", parser.error_.c_str());
        return 1;
    }
    printTodos(parser.builder_.GetBufferPointer());
    
    // numeric
    if (!parser.ParseJson(R"({"1":[{"1":"CgAAAAAAAACZ","2":1491921868559,"3":"world","4":false}]})", true))
    {
        fprintf(stderr, "Failed to parse numeric json.\n%s\n", parser.error_.c_str());
        return 1;
    }
    printTodos(parser.builder_.GetBufferPointer());
    */
    
    App app(host, port);
    bool ok = app.parser.Parse(todo_user_schema);
    if (!ok)
        fprintf(stderr, "Could not load schema.\n");
    
    return !ok ? 1 : app.show();
}
