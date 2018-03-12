#pragma once

#include <vector>
#include <forward_list>
#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/picture.hpp>

namespace ui {

/* msvc2015 fails to compile this -> C3805
enum class Msg
{
    SUCCESS,
    ERROR,
    WARNING
};
*/

enum class Msg
{
    $SUCCESS,
    $ERROR,
    $WARNING
};

struct MsgColors
{
    const nana::color success_fg;
    const nana::color success_bg;
    const nana::color error_fg;
    const nana::color error_bg;
    const nana::color warning_fg;
    const nana::color warning_bg;
    const nana::color close_fg;
    
    MsgColors(
            unsigned success_fg, unsigned success_bg,
            unsigned error_fg, unsigned error_bg,
            unsigned warning_fg, unsigned warning_bg,
            unsigned close_fg):
        success_fg(nana::color_rgb(success_fg)),
        success_bg(nana::color_rgb(success_bg)),
        error_fg(nana::color_rgb(error_fg)),
        error_bg(nana::color_rgb(error_bg)),
        warning_fg(nana::color_rgb(warning_fg)),
        warning_bg(nana::color_rgb(warning_bg)),
        close_fg(nana::color_rgb(close_fg))
    {
        
    }
    
    static const MsgColors DEFAULT;
};

const MsgColors MsgColors::DEFAULT {
    0x52A954, 0xDEFCD5,
    0xA95252, 0xF1D7D7,
    0x96904D, 0xF6F3D5,
    0x777777
};

// not in the widget api
inline void visible(nana::widget& w, bool on)
{
    nana::API::show_window(w.handle(), on);
}

enum class WindowFlags : uint8_t
{
    TASKBAR = 1,
    FLOATING = 2,
    NO_ACTIVATE = 4,
    MINIMIZE = 8,
    MAXIMIZE = 16,
    SIZABLE = 32,
    DECORATION = 64,
    
    DEFAULT = DECORATION | TASKBAR | MINIMIZE | MAXIMIZE | SIZABLE,
    STATIC = DECORATION | TASKBAR | MINIMIZE
};

inline uint8_t operator| (uint8_t a, WindowFlags b)
{
    return a | static_cast<uint8_t>(b);
}
inline uint8_t operator& (uint8_t a, WindowFlags b)
{
    return a & static_cast<uint8_t>(b);
}

struct Form;
Form* root{ nullptr };

struct Form : nana::form
{
    Form(nana::rectangle rect, uint8_t flags = uint8_t(WindowFlags::DEFAULT), unsigned bg = 0xFFFFFF): nana::form(rect,
        nana::appearance(
            0 != (flags & WindowFlags::DECORATION),
            0 != (flags & WindowFlags::TASKBAR),
            0 != (flags & WindowFlags::FLOATING),
            0 != (flags & WindowFlags::NO_ACTIVATE),
            0 != (flags & WindowFlags::MINIMIZE),
            0 != (flags & WindowFlags::MAXIMIZE),
            0 != (flags & WindowFlags::SIZABLE)
        )
    )
    {
        root = this;
        bgcolor(nana::color_rgb(bg));
        events().unload([](const nana::arg_unload& arg) {
            nana::API::exit();
        });
    }
};

struct SubForm : nana::form
{
    const bool modal;
    SubForm(nana::rectangle rect,
            const std::string& title = "",
            bool modal = true,
            uint8_t flags = uint8_t(WindowFlags::DECORATION),
            unsigned bg = 0xFFFFFF): nana::form(*ui::root, rect,
        nana::appearance(
            0 != (flags & WindowFlags::DECORATION),
            0 != (flags & WindowFlags::TASKBAR),
            0 != (flags & WindowFlags::FLOATING),
            0 != (flags & WindowFlags::NO_ACTIVATE),
            0 != (flags & WindowFlags::MINIMIZE),
            0 != (flags & WindowFlags::MAXIMIZE),
            0 != (flags & WindowFlags::SIZABLE)
        )
    ), modal(modal)
    {
        caption(title);
        bgcolor(nana::color_rgb(bg));
        events().unload([this](const nana::arg_unload& arg) {
            arg.cancel = true;
            hide();
            if (this->modal)
                nana::API::window_enabled(*root, true);
        });
    }
    
    void popTo(nana::window target, int y = 0)
    {
        auto pos = nana::API::window_position(target);
        pos.y += y;
        nana::API::move_window(*this, pos);
        show();
        if (modal)
            nana::API::window_enabled(*root, false);
    }
};

struct Place : nana::place
{
    Place(nana::widget& owner, const char* layout) : nana::place(owner)
    {
        div(layout);
    }
};

struct Icon : nana::picture
{
    Icon(nana::widget& owner, nana::paint::image icon, bool cursor_hand = false) : nana::picture(owner)
    {
        load(icon);
        transparent(true);
        
        if (!cursor_hand)
            return;
        
        events().mouse_move([this](const nana::arg_mouse& arg) {
            cursor(nana::cursor::hand);
        });
    }
    Icon(nana::widget& owner, const char* icon, bool cursor_hand = false):
        Icon(owner, nana::paint::image(icon), cursor_hand)
    {
        
    }
};

struct Panel : nana::panel<false>
{
    nana::place place{ *this };
    
    Panel(nana::widget& owner, const char* layout) : nana::panel<false>(owner)
    {
        place.div(layout);
    }
};

struct ToggleIcon : Panel
{
    Icon on_;
    Icon off_;
    
    ToggleIcon(nana::widget& owner, nana::paint::image icon_on, nana::paint::image icon_off):
        Panel(owner, "<on_><off_>"),
        on_(*this, icon_on, true),
        off_(*this, icon_off, true)
    {
        place["on_"] << on_;
        place["off_"] << off_;
        place.collocate();
        place.field_display("off_", false);
    }
    ToggleIcon(nana::widget& owner, const char* icon_on, const char* icon_off):
        ToggleIcon(owner, nana::paint::image(icon_on), nana::paint::image(icon_off))
    {
        
    }
    
    void update(bool on)
    {
        place.field_display("on_", on);
        place.field_display("off_", !on);
        place.collocate();
    }
};

struct DeferredPanel : nana::panel<false>
{
    const char* const layout;
    nana::place place;
    
    DeferredPanel(const char* layout) : nana::panel<false>(), layout(layout)
    {
        
    }
    
private:
    void _m_complete_creation() override
    {
        place.bind(*this);
        place.div(layout);
        
        //transparent(true);
    }
};

struct BgPanel : nana::panel<true>
{
    nana::place place{ *this };
    
    BgPanel(nana::widget& owner, const char* layout, unsigned bg = 0, unsigned fg = 0) : nana::panel<true>(owner)
    {
        place.div(layout);
        if (bg)
            bgcolor(nana::color_rgb(bg));
        if (fg)
            fgcolor(nana::color_rgb(fg));
    }
};

struct MsgPanel : BgPanel
{
    const MsgColors colors;
    nana::label msg_{ *this, "" };
    nana::label close_{ *this, "<bold target=\"0\"> x </>" };
    
    MsgPanel(nana::widget& owner, const MsgColors& colors) : BgPanel(owner,
        "margin=[3,2,1,3]"
        "<msg_>"
        "<close_ weight=20>"
    ),  colors(colors)
    {
        auto listener = [this](nana::label::command cmd, const std::string& target) {
            if (nana::label::command::click == cmd)
                hide();
        };
        
        place["msg_"] << msg_
                .text_align(nana::align::left)
                .transparent(true);
        
        place["close_"] << close_
                .text_align(nana::align::right)
                .add_format_listener(listener)
                .format(true)
                .transparent(true);
        
        close_.fgcolor(colors.close_fg);
        
        place.collocate();
        
        // initially hidden
        hide();
    }
    
    void update(const std::string& msg, Msg type = ui::Msg::$ERROR)
    {
        msg_.caption(msg);
        
        switch (type)
        {
            case ui::Msg::$SUCCESS:
                msg_.fgcolor(colors.success_fg);
                bgcolor(colors.success_bg);
                break;
            case ui::Msg::$ERROR:
                msg_.fgcolor(colors.error_fg);
                bgcolor(colors.error_bg);
                break;
            case ui::Msg::$WARNING:
                msg_.fgcolor(colors.warning_fg);
                bgcolor(colors.warning_bg);
                break;
        }
        
        show();
    }
};

template <typename T, typename W>
struct List : Panel
{
private:
    std::forward_list<W> items;
    std::vector<W*> array;
    nana::color selected_bg;
    int selected_idx{ -1 };
    
public:    
    List(nana::widget& owner, const char* layout = nullptr, unsigned selected_bg = 0xF3F3F3):
        Panel(owner, layout ? layout : "margin=[5,0] <items_ vert>"),
        selected_bg(nana::color_rgb(selected_bg))
    {
        
    }
    
    void collocate(int pageSize = 10)
    {
        for (int i = 0; i < pageSize; i++)
        {
            items.emplace_front(*this);
            place["items_"] << items.front();
            array.push_back(&items.front());
        }
        
        place.collocate();
    }
    
    int size()
    {
        return array.size();
    }
    
    void populate(int idx, T* pojo)
    {
        array[idx]->update(pojo);
    }
    
    bool trySelect(int idx)
    {
        int prev_idx = selected_idx;
        if (idx == prev_idx)
            return false;
        
        if (idx == -1)
        {
            // deselect
            selected_idx = idx;
            array[prev_idx]->bgcolor(nana::colors::white);
            return true;
        }
        
        if (idx < 0 || idx >= array.size())
            return false;
        
        selected_idx = idx;
        
        if (prev_idx != -1)
            array[prev_idx]->bgcolor(nana::colors::white);
        
        array[idx]->bgcolor(selected_bg);
        return true;
    }
};
    
} // ui
