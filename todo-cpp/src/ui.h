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
};

// not in the widget api
inline void visible(nana::widget& w, bool on)
{
    nana::API::show_window(w.handle(), on);
}

struct Form : nana::form
{
    Form(nana::rectangle rect, unsigned bg, const char* title = nullptr) : nana::form(rect)
    {
        bgcolor(nana::color_rgb(bg));
        if (title)
            caption(title);
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
    Icon(nana::widget& owner, const char* icon, bool cursor_hand = false) : nana::picture(owner)
    {
        load(nana::paint::image(icon));
        transparent(true);
        
        if (!cursor_hand)
            return;
        
        events().mouse_move([this](const nana::arg_mouse& arg) {
            cursor(nana::cursor::hand);
        });
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

template <typename T, typename W>
struct List : Panel
{
private:
    std::forward_list<W> items;
    std::vector<W*> array;
    nana::color selected_bg;
    int selected_idx{ -1 };
    
public:    
    List(nana::widget& owner, const char* layout = nullptr, unsigned selected_bg = 0xF3F3F3, int pageSize = 10):
        Panel(owner, layout ? layout : "margin=[5,0] <items_ vert>"),
        selected_bg(nana::color_rgb(selected_bg))
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
    
    bool select(int idx)
    {
        int prev_idx = selected_idx;
        if (idx == prev_idx || idx < 0 || idx >= array.size())
            return false;
        
        selected_idx = idx;
        
        if (prev_idx != -1)
            array[prev_idx]->bgcolor(nana::colors::white);
        
        array[idx]->bgcolor(selected_bg);
        return true;
    }
};
    
} // ui
