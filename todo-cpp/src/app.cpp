#include <forward_list>
#include <thread>

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>

#include "util.h"
#include "rpc.h"
#include "ui.h"
#include "app.h"

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

nana::listbox::oresolver& operator << (nana::listbox::oresolver& orr, const todo::user::Todo* todo)
{
    orr << todo->title()->c_str();
    orr << (todo->completed() ? "*" : " ");
    return orr;
}

static const int MARGIN = 5,
        WIDTH = 1005,
        HEIGHT = 710,
        // page
        PAGE_SIZE = 25,
        MULTIPLIER = 2,
        // listbox
        LB_OUTER = MARGIN * 2,
        LB_HEIGHT = HEIGHT - LB_OUTER,
        LB_WIDTH = WIDTH - LB_OUTER,
        // panel
        LB_PANEL_WIDTH = LB_WIDTH - (MARGIN * 3),
        // inner
        LB_FIELDS = 2,
        LB_INNER = MARGIN * 3 * LB_FIELDS,
        COMPLETED_WIDTH = 20,
        TITLE_WIDTH = LB_WIDTH - LB_INNER - COMPLETED_WIDTH;

struct TodoItem : nana::listbox::inline_notifier_interface
{
    inline_indicator* ind_ { nullptr };
    index_type pos_;
    
    ui::DeferredPanel pnl_ {
        "margin=[1,10]"
        "<lbl_ weight=60%>"
        "<txt_ weight=30% margin=[0,5]>"
        "<btn_ weight=10%>" 
    };
    nana::label lbl_;
    nana::textbox txt_;
    nana::button btn_;
    
    bool initialized{ false };
    
private:
    void create(nana::window wd) override
    {
        auto $selected = [this]() {
            ind_->selected(pos_);
        };
        auto $key_press = [this](const nana::arg_keyboard& arg) {
            bool upward = false;
            switch(arg.key)
            {
                case nana::keyboard::os_arrow_up:
                    upward = true;
                case nana::keyboard::os_arrow_down:
                    if (!arg.ctrl)
                        dynamic_cast<nana::listbox&>(ind_->host()).move_select(upward);
                    else
                        dynamic_cast<nana::listbox&>(ind_->host()).at(0).at(upward ? 0 : PAGE_SIZE - 1).select(true);
                    break;
            }
        };
        
        pnl_.create(wd);
        
        // label
        lbl_.create(pnl_);
        lbl_.transparent(true)
            .format(true)
            .events().click($selected);
        lbl_.events().key_press($key_press);
        pnl_.place["lbl_"] << lbl_;
        
        // textbox
        txt_.create(pnl_);
        txt_.events().click($selected);
        pnl_.place["txt_"] << txt_;
        
        // button
        btn_.create(pnl_);
        btn_.caption("Delete");
        btn_.events().click([this]
        {
            // TODO delete the item when button is clicked
            //auto& lsbox = dynamic_cast<nana::listbox&>(ind->host());
            //lsbox.erase(lsbox.at(pos_));
        });
        pnl_.place["btn_"] << btn_;
    }
    
    void activate(inline_indicator& ind, index_type pos) override
    {
        ind_ = &ind;
        pos_ = pos;
    }
    void resize(const nana::size& d) override
    {
        pnl_.size(d);
    }
    void set(const std::string& value) override
    {
        bool visible = !value.empty();
        if (!visible)
        {
            // check if it is visible
            if (pnl_.place.field_visible("btn_"))
            {
                pnl_.place.field_visible("btn_", visible);
                //pnl_.place.field_visible("txt_", visible);
                pnl_.place.field_visible("lbl_", visible);
            }
            return;
        }
        
        // TODO fill data
        
        // check if not already visible
        if (!pnl_.place.field_visible("btn_"))
        {
            pnl_.place.field_visible("btn_", visible);
            //pnl_.place.field_visible("txt_", visible);
            pnl_.place.field_visible("lbl_", visible);
        }
        
        txt_.caption(value);
    }
    void notify_status(status_type status, bool on) override
    {
        /*switch (status)
        {
            case status_type::selecting:
                break;
            case status_type::checking:
                break;
        }*/
    }
    //Determines whether to draw the value of sub item
    //e.g, when the inline widgets covers the whole background of the sub item,
    //it should return false to avoid listbox useless drawing
    bool whether_to_draw() const override
    {
        return false;
    }
};

struct Home : ui::Panel
{
    nana::listbox lb{ *this, { 0, 0, LB_WIDTH, LB_HEIGHT } };
    
    bool initialized { false };
    
    Home(ui::Panel& owner, const char* field, const bool display = true) : ui::Panel(owner, 
        "<lb_>"
    )
    {
        lb.show_header(false);
        lb.enable_single(true, true);
        
        // 1-column inline widgets
        lb.append_header("", LB_PANEL_WIDTH);
        lb.at(0).inline_factory(0, nana::pat::make_factory<TodoItem>());
        
        // 2-column text-only
        //lb.append_header("", TITLE_WIDTH);
        //lb.append_header("", COMPLETED_WIDTH);
        
        place["lb_"] << lb;
        place.collocate();
        place.field_visible("lb_", false);
        
        owner.place[field] << *this;
        if (!display)
            owner.place.field_display(field, false);
    }
    
    void appendTodos(void* flatbuf)
    {
        auto wrapper = flatbuffers::GetRoot<todo::user::Todo_PList>(flatbuf);
        auto plist = wrapper->p();
        auto slot = lb.at(0);
        
        // 1-column inline widgets
        bool makeVisible = false;
        if (!initialized)
        {
            for (int i = 0; i < PAGE_SIZE; ++i)
                slot.append({ "" });
            
            initialized = makeVisible = true;
        }
        
        // TODO fill data
        for (int i = 0, len = PAGE_SIZE/*plist->size()*/; i < len; i++)
        {
            slot.at(i).text(0, "gg");
        }
        
        if (makeVisible)
            place.field_visible("lb_", true);
        
        // 2-column text-only
        //for (int i = 0, len = plist->size(); i < len; i++)
        //    slot.append(plist->Get(i));
    }
};

struct About : ui::Panel
{
    nana::label text{ *this, "about" };
    
    About(ui::Panel& owner, const char* field, const bool display = true) : ui::Panel(owner, 
        "<text_>"
    )
    {
        //text.bgcolor(nana::color_rgb(0xFCFCFC));
        
        place["text_"] << text;
        place.collocate();
        
        owner.place[field] << *this;
        if (!display)
            owner.place.field_display(field, false);
    }
};

static const char MALFORMED_MESSAGE[] = "Malformed message.";

static const char* LINKS[] = {
    "<color=0x0080FF size=11 target=\"content_0\">Home</>",
    "<color=0x0080FF size=11 target=\"content_1\">About</>"
};

static const int IDLE_INTERVAL = 10000,
        RECONNECT_INTERVAL = 5000;

struct App : rpc::Base
{
    std::forward_list<nana::label> links;
    int current_selected{ 0 };
    std::string current_target{ "content_0" };
    
    ui::Form fm{ {273, 0, WIDTH, HEIGHT}, 0xFFFFFF };
    nana::place place{ fm };
    nana::label bottom{ fm, "Copyright 2018 <color=0x0080FF>David Yu</>" };
    ui::Panel content{ fm,
        "vert"
        "<content_0>"
        "<content_1>"
    };
    Home home{ content, "content_0" };
    About about{ content, "content_1", false };
    
    bool fetched_initial{ false };
    
    App(const char* host, int port, bool secure, const std::string& title) : rpc::Base(host, port, secure)
    {
        fm.caption(title);
        place.div(
            "vert margin=5"
            "<header_ weight=20 margin=[0,30%]>"
            "<content_>"
            "<footer_ weight=20>"
        );
        
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
            home.appendTodos(parser.builder_.GetBufferPointer());
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
            loop->loop(IDLE_INTERVAL);
        }
        else if (!connect(false))
        {
            // TODO show error
            
            loop->loop(RECONNECT_INTERVAL);
        }
        /*else
        {
            fprintf(stdout, "connected\n");
        }*/
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

namespace todo {

int run(int argc, char* argv[], const std::string& title)
{
    int port;
    bool secure;
    const char* host = util::resolveEndpoint(argc > 1 ? argv[1] : nullptr, &port, &secure);
    
    if (host == nullptr)
    {
        fprintf(stderr, "Invalid endpoint %s\n", argv[1]);
        return 1;
    }
    
    App app(host, port, secure, title);
    
    if (!app.parser.Parse(todo_user_schema))
    {
        fprintf(stderr, "Could not load schema.\n");
        return 1;
    }
    
    return app.show();
}

} // todo


