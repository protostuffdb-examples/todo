#pragma once

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/panel.hpp>
#include <nana/gui/widgets/picture.hpp>

namespace ui {
    
    struct Form : nana::form
    {
        Form(nana::rectangle rect, unsigned bg, const char* title = nullptr) : nana::form(rect)
        {
            bgcolor(nana::color_rgb(bg));
            if (title)
                caption(title);
        }
    };
    
    struct Panel : nana::panel<true>
    {
        nana::place place{ *this };
        
        Panel(nana::widget& owner, const char* layout) : nana::panel<true>(owner)
        {
            place.div(layout);
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
    
} // ui
