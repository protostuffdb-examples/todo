#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/label.hpp>
#include <forward_list>

#include <flatbuffers/flatbuffers.h>
#include <flatbuffers/idl.h>

#include "UrlRequest.h"
#include "ui.h"

#include "../g/user/fbs_schema.h"

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

static const char* LINKS[] = {
    "<color=0x0080FF size=11 target=\"content_0\">Home</>",
    "<color=0x0080FF size=11 target=\"content_1\">Test</>",
    "<color=0x0080FF size=11 target=\"content_2\">About</>"
};

struct App
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
    
    App()
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
    
    flatbuffers::Parser parser;
    if (!parser.Parse(todo_user_schema) || !parser.SetRootType("Todo_PList"))
    {
        fprintf(stderr, "Could not load schema.\n");
        return 1;
    }
    
    UrlRequest req;
    req.host(host).port(port).uri("/todo/user/Todo/list")
        .method("POST")
        .addHeader("Content-Type: application/json")
        .body(R"({"1":true,"2":31})");
    
    auto res = std::move(req.perform());
    if (200 != res.statusCode())
    {
        fprintf(stderr, "Failed request.\n");
        return 1;
    }
    
    auto body = res.body();
    if (body[0] != '+')
    {
        fprintf(stderr, "Failed.\n%s\n", body.c_str());
        return 1;
    }
    
    // remove suffix: ]
    body[body.size() - 1] = '\0';
    
    // remove prefix: +[0,
    const char* json = body.c_str() + 4;
    
    if (!parser.ParseJson(json, true))
    {
        fprintf(stderr, "Could not parse json.\n%s\n", json);
        return 1;
    }
    
    fprintf(stdout, "body:\n%s\n", json);
    
    App app;
    return app.show();
}
