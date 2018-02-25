#include <forward_list>
#include <thread>

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/widgets/button.hpp>

//#include <asap/asap.h>

#include "pstore.h"
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
        #ifdef WIN32
        HEIGHT = 750,
        #else
        HEIGHT = 710,
        #endif
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

// hack
struct TodoItem;
static std::vector<TodoItem*> todo_items;

using TodoStore = coreds::PojoStore<Todo, todo::user::Todo>;

struct TodoItem : nana::listbox::inline_notifier_interface
{
    inline_indicator* ind_ { nullptr };
    index_type pos_;
    
    ui::DeferredPanel pnl_ {
        "margin=[1,10]"
        "<title_>"
        "<ts_ weight=120 margin=[0,5]>"
        "<txt_ weight=200 margin=[0,5,0,0]>"
        "<btn_ weight=25>" 
    };
    nana::label title_;
    nana::label ts_;
    nana::textbox txt_;
    nana::button btn_;
    
    Todo* pojo{ nullptr };
    TodoStore* store{ nullptr };
    
    TodoItem()
    {
        todo_items.push_back(this);
    }
    
    void update(Todo* message)
    {
        pojo = message;
        if (message == nullptr)
        {
            pnl_.hide();
            return;
        }
        
        title_.caption(pojo->title);
        
        std::string timeago;
        timeago.reserve(16); // just moments ago
        util::appendTimeagoTo(timeago, pojo->ts);
        ts_.caption(timeago);
    }
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
        
        // title
        title_.create(pnl_);
        title_.transparent(true)
            .events().click($selected);
        title_.events().key_press($key_press);
        pnl_.place["title_"] << title_;
        
        // ts
        ts_.create(pnl_);
        ts_.transparent(true)
            .text_align(nana::align::right)
            .events().click($selected);
        ts_.events().key_press($key_press);
        pnl_.place["ts_"] << ts_;
        
        // textbox
        txt_.create(pnl_);
        txt_.events().click($selected);
        pnl_.place["txt_"] << txt_;
        
        // button
        btn_.create(pnl_);
        btn_.caption("x");
        //btn_.events().key_press($key_press);
        btn_.events().click([this]
        {
            ind_->selected(pos_);
            // TODO delete the item when button is clicked
            //auto& lsbox = dynamic_cast<nana::listbox&>(ind->host());
            //lsbox.erase(lsbox.at(pos_));
        });
        pnl_.place["btn_"] << btn_;
    }
    void notify_status(status_type status, bool on) override
    {
        /*if (pojo && on && status == status_type::selecting)
        {
            fprintf(stderr, "selected %s\n", pojo->title()->c_str());
        }*/
    }
    void activate(inline_indicator& ind, index_type pos) override { ind_ = &ind; pos_ = pos; }
    void resize(const nana::size& d) override { pnl_.size(d); }
    void set(const std::string& value) override {}
    bool whether_to_draw() const override { return false; }
};

static const char* SORT_TOGGLE[] = {
    " <color=0x0080FF size=11 target=\"0\"> dsc </>",
    " <color=0x0080FF size=11 target=\"1\"> asc </>",
};

static const unsigned SUCCESS_BG = 0xDEFCD5;
static const std::string SUCCESS_PFX = "<color=52A954 target=\"8\">";

static const unsigned ERROR_BG = 0xF1D7D7;
static const std::string ERROR_PFX = "<color=A95252 target=\"8\">";

static const unsigned WARNING_BG = 0xF6F3D5;
static const std::string WARNING_PFX = "<color=96904D target=\"8\">";

static const std::string MSG_SFX = "</>";

struct Home : ui::Panel
{
    nana::textbox search_{ *this };
    
    nana::label add_{ *this,
        "  "
        "<bold color=0x0080FF size=11 target=\"2\"> + </>"
    };
    nana::label sort_{ *this, SORT_TOGGLE[0] };
    nana::label refresh_{ *this,
        "  "
        "<color=0x0080FF size=11 target=\"3\">refresh</>"
    };
    
    nana::label nav_{ *this,
        "<color=0x0080FF size=11 target=\"4\">\\<\\<</>"
        "     "
        "<color=0x0080FF size=11 target=\"5\"> \\< </>"
        "     "
        "<color=0x0080FF size=11 target=\"6\"> \\> </>"
        "     "
        "<color=0x0080FF size=11 target=\"7\">\\>\\></>"
    };
    
    nana::label msg_{ *this, "" };
    
    nana::listbox list_{ *this, { 0, 25 + MARGIN, unsigned(LB_WIDTH), unsigned(LB_HEIGHT - (25 + MARGIN)) } };
    
    bool desc{ true };
    
    bool initialized{ false };
    
    int item_offset;
    
    Home(ui::Panel& owner, const char* field, const bool display = true) : ui::Panel(owner,
        "vert"
        "<horizontal weight=25"
          "<search_ weight=200>"
          "<add_ weight=40>"
          "<sort_ weight=40>"
          "<refresh_ weight=80>"
          "<msg_>"
          "<nav_ weight=160>"
        ">"
        "<list_>"
    )
    {
        place["search_"] << search_.tip_string("Todo");
        
        // ctrls
        auto listener = [this](nana::label::command cmd, const std::string& target) {
            if (nana::label::command::click != cmd)
                return;
            
            int type = std::atoi(target.c_str());
            switch (type)
            {
                case 0:
                case 1:
                    desc = 0 == (type ^ 1);
                    sort_.caption(SORT_TOGGLE[type ^ 1]);
                    break;
                case 8:
                    place.field_visible("msg_", false);
                    break;
                // TODO
            }
        };
        place["add_"] << add_
                .text_align(nana::align::center)
                .add_format_listener(listener)
                .format(true);
        
        place["sort_"] << sort_
                .text_align(nana::align::center)
                .add_format_listener(listener)
                .format(true);
        
        place["refresh_"] << refresh_
                .text_align(nana::align::center)
                .add_format_listener(listener)
                .format(true);
        
        place["msg_"] << msg_
                .text_align(nana::align::left)
                .add_format_listener(listener)
                .format(true);
        
        place["nav_"] << nav_
                .text_align(nana::align::right)
                .add_format_listener(listener)
                .format(true);
        
        // listbox
        list_.show_header(false);
        list_.enable_single(true, true);
        
        // 1-column inline widgets
        list_.append_header("", LB_PANEL_WIDTH);
        list_.at(0).inline_factory(0, nana::pat::make_factory<TodoItem>());
        
        // 2-column text-only
        //list_.append_header("", TITLE_WIDTH);
        //list_.append_header("", COMPLETED_WIDTH);
        
        place["list_"] << list_;
        place.collocate();
        place.field_visible("msg_", false);
        place.field_visible("list_", false);
        
        owner.place[field] << *this;
        if (!display)
            owner.place.field_display(field, false);
    }
    
    void init()
    {
        if (initialized)
            return;
        
        item_offset = todo_items.size();
        
        auto slot = list_.at(0);
        for (int i = 0; i < PAGE_SIZE; ++i)
            slot.append({ "" });
        
        place.field_visible("list_", true);
        initialized = true;
        place.field_visible("list_", false);
    }
    
    void populate(int idx, Todo* pojo)
    {
        todo_items[item_offset + idx]->update(pojo);
    }
    
    void show(const std::string& msg, ui::Msg type = ui::Msg::ERROR)
    {
        std::string buf;
        
        switch (type)
        {
            case ui::Msg::SUCCESS:
                msg_.bgcolor(nana::color_rgb(SUCCESS_BG));
                buf += SUCCESS_PFX;
                buf += msg;
                buf += MSG_SFX;
                break;
            case ui::Msg::ERROR:
                msg_.bgcolor(nana::color_rgb(ERROR_BG));
                buf += ERROR_PFX;
                buf += msg;
                buf += MSG_SFX;
                break;
            case ui::Msg::WARNING:
                msg_.bgcolor(nana::color_rgb(WARNING_BG));
                buf += WARNING_PFX;
                buf += msg;
                buf += MSG_SFX;
                break;
        }
        
        msg_.caption(buf);
        place.field_visible("msg_", true);
    }
    
    /*void appendTodos(void* flatbuf)
    {
        auto wrapper = flatbuffers::GetRoot<todo::user::Todo_PList>(flatbuf);
        auto plist = wrapper->p();
        
        if (plist == nullptr || 0 == plist->size())
            return;
        
        auto slot = list_.at(0);
        
        // lock for ui updates
        nana::internal_scope_guard lock;
        
        // 1-column inline widgets
        if (!initialized)
        {
            item_offset = todo_items.size();
            
            for (int i = 0; i < PAGE_SIZE; ++i)
                slot.append({ "" });
            
            place.field_visible("list_", true);
            initialized = true;
        }
        
        //slot.at(i).text(0, "gg");
        int i = 0, len = std::min(PAGE_SIZE, static_cast<int>(plist->size()));
        for (; i < len; i++) todo_items[item_offset + i]->update(plist->Get(i));
        for (; i < PAGE_SIZE; i++) todo_items[item_offset + i]->update(nullptr);
        
        // 2-column text-only
        //for (int i = 0, len = plist->size(); i < len; i++)
        //    slot.append(plist->Get(i));
    }*/
};

struct About : ui::Panel
{
    nana::label text_{ *this, "about" };
    
    About(ui::Panel& owner, const char* field, const bool display = true) : ui::Panel(owner, 
        "<text_>"
    )
    {
        //text.bgcolor(nana::color_rgb(0xFCFCFC));
        
        place["text_"] << text_;
        place.collocate();
        
        owner.place[field] << *this;
        if (!display)
            owner.place.field_display(field, false);
    }
};

static const char* LINKS[] = {
    "<color=0x0080FF size=11 target=\"content_0\">Home</>",
    "<color=0x0080FF size=11 target=\"content_1\">About</>"
};

static const int IDLE_INTERVAL = 10000,
        RECONNECT_INTERVAL = 5000;

struct App : rpc::Base
{
    ui::Form fm{ {273, 0, unsigned(WIDTH), unsigned(HEIGHT)}, 0xFFFFFF };
    
    ui::Place place{ fm, 
        "vert margin=5"
        "<header_ weight=20 margin=[0,30%]>"
        "<content_>"
        "<footer_ weight=20>"
    };
    
    nana::label footer_{ fm, "Copyright 2018 <color=0x0080FF>David Yu</>" };
    
    ui::Panel content_{ fm,
        "vert"
        "<content_0>"
        "<content_1>"
    };
    Home home{ content_, "content_0" };
    About about{ content_, "content_1", false };
    
    std::forward_list<nana::label> links;
    std::string current_target{ "content_0" };
    int current_selected{ 0 };
    
    const brynet::net::HttpSession::PTR* session;
    TodoStore store;
    
    bool fetched_initial{ false };
    
    App(const rpc::Config config, const char* title) : rpc::Base(config)
    {
        fm.caption(title ? title : "Todo App");
    }

private:
    void links$$(const std::string& target)
    {
        int selected = target.back() - 48;
        if (selected == current_selected)
            return;
        
        // hide current
        content_.place.field_display(current_target.c_str(), false);
        
        // set current
        current_selected = selected;
        current_target[current_target.size() - 1] = target.back();
        
        // show
        content_.place.field_display(target.c_str(), true);
        content_.place.collocate();
    }
    
    void onHttpData(const brynet::net::HTTPParser& httpParser,
            const brynet::net::HttpSession::PTR& session) override
    {
        auto body = httpParser.getBody();
        if (!store.isLoading())
        {
            // some other request
        }
        else if (rpc::parseJson(body, "Todo_PList", parser, store.errmsg))
        {
            //home.appendTodos(parser.builder_.GetBufferPointer());
            //fetched_initial = true;
            if (!fetched_initial)
                home.init();
            
            if (store.cbFetchSuccess(
                flatbuffers::GetRoot<todo::user::Todo_PList>(parser.builder_.GetBufferPointer())->p()
            ))
            {
                fetched_initial = true;
            }
        }
        else
        {
            store.cbFetchFailed();
            
            nana::internal_scope_guard lock;
            home.show(store.errmsg);
        }
    }
    
    void onHttpOpen(const brynet::net::HttpSession::PTR& session) override
    {
        this->session = &session;
        if (!fetched_initial)
            store.fetchNewer();
        //if (!fetched_initial)
        //    post(session, "/todo/user/Todo/list", R"({"1":true,"2":31})");
    }
    
    void onHttpClose(const brynet::net::HttpSession::PTR& session) override
    {
        this->session = nullptr;
        fd = SOCKET_ERROR;
        //connect(true);
        
        if (store.isLoading())
        {
            store.errmsg = "Fetch failed.";
            store.cbFetchFailed();
            
            nana::internal_scope_guard lock;
            home.show(errmsg);
        }
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

public:
    bool init(coreds::Opts opts)
    {
        store.init(opts);
        return parser.Parse(todo_user_schema);
    }
    void show()
    {
        store.$fnKey = [](const Todo& pojo) {
            return pojo.key.c_str();
        };
        store.$fnKeyFB = [](const todo::user::Todo* message) {
            return message->key()->c_str();
        };
        store.$fnUpdate = [](Todo& pojo, const todo::user::Todo* message) {
            // TODO conditional assign on strings
            pojo.title = message->title()->str();
            pojo.completed = message->completed();
        };
        store.$fnPopulate = [this](int idx, Todo* pojo) {
            home.populate(idx, pojo);
        };
        store.$fnCall = [this](std::function<void()> op) {
            nana::internal_scope_guard lock;
            home.place.field_visible("list_", false);
            op();
            home.place.field_visible("list_", true);
        };
        store.$fnEvent = [this](coreds::EventType type, bool on) {
            switch (type)
            {
                case coreds::EventType::VISIBLE:
                    home.place.field_visible("list_", on);
                    break;
            }
        };
        store.$fnFetch = [this](coreds::ParamRangeKey prk) {
            if (session == nullptr)
                return false;
            
            std::string buf;
            prk.stringifyTo(buf);
            post(*session, "/todo/user/Todo/list", buf.c_str());
            return true;
        };
        
        // header
        auto listener = [this](nana::label::command cmd, const std::string& target) {
            if (nana::label::command::click == cmd)
                links$$(target);
        };
        
        for (auto text : LINKS)
        {
            links.emplace_front(fm.handle());
            
            place["header_"] << links.front()
                .text_align(nana::align::center)
                .format(true)
                .add_format_listener(listener)
                .caption(text);
        }
        
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
    
    todo_items.reserve(PAGE_SIZE);
    
    coreds::Opts opts;
    opts.pageSize = PAGE_SIZE;
    opts.multiplier = MULTIPLIER;
    
    App app(config, title);
    
    if (!app.init(opts))
    {
        fprintf(stderr, "Could not load schema.\n");
        return 1;
    }
    
    app.show();
    nana::exec();
    return 0;
}

} // todo


