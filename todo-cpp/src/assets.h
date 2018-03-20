#pragma once

#include <nana/gui/wvl.hpp>
#include <coreds/nana/ui.h>

namespace msgs {

const std::string validation_required("All required fields must be provided.");

} // msgs

namespace colors {

const nana::color primary(nana::color_rgb(0x0080FF));
const nana::color lgray(nana::color_rgb(0xF3F3F3));
const nana::color border(nana::color_rgb(0xDEDEDE));

} // colors

namespace icons {

const nana::paint::image arrow_up("assets/png/arrow-up.png");
const nana::paint::image arrow_down("assets/png/arrow-down.png");
const nana::paint::image angle_left("assets/png/angle-left.png");
const nana::paint::image angle_right("assets/png/angle-right.png");
const nana::paint::image angle_double_left("assets/png/angle-double-left.png");
const nana::paint::image angle_double_right("assets/png/angle-double-right.png");

const nana::paint::image circle("assets/png/circle.png");
const nana::paint::image circle_empty("assets/png/circle-empty.png");

const nana::paint::image cw("assets/png/cw.png");
const nana::paint::image plus("assets/png/plus.png");

} // icons

namespace util {

const int MARGIN = 5,
        LEFT = 273,
        MAX_HEIGHT = 842, // 900 - 58
        WIDTH = 1005,
        #ifdef WIN32
        DEF_HEIGHT = 700,
        POP_OFFSET = 67,
        #else
        DEF_HEIGHT = 710, // 768 - 58
        POP_OFFSET = 63,
        #endif
        // page
        PAGE_SIZE = 20,
        MULTIPLIER = 2,
        // listbox
        LB_OUTER = MARGIN * 2,
        //LB_HEIGHT = DEF_HEIGHT - LB_OUTER,
        LB_WIDTH = WIDTH - LB_OUTER,
        // panel
        LB_PANEL_WIDTH = LB_WIDTH - (MARGIN * 3);

inline int maxHeight(nana::size screenSize)
{
    int h = screenSize.height;
    return h > util::MAX_HEIGHT ? std::max(h - 58, util::MAX_HEIGHT) : util::DEF_HEIGHT;
}

struct ScreenConfig
{
    const nana::size size;
    const int height;
    const int lb_height;
    const bool hd;
    ScreenConfig(nana::size pms = nana::screen::primary_monitor_size()):
        size(pms),
        height(std::min(maxHeight(size), 1022)), // 1080 - 58 - hd max height
        lb_height(height - LB_OUTER),
        hd(height >= MAX_HEIGHT)
    {
        
    }
};

ScreenConfig* sc{ nullptr };

} // util

namespace fonts {

/*
const nana::paint::font r8("", 8); // max ph: 16
const nana::paint::font r9("", 9); // max ph: 18
const nana::paint::font r10("", 10); // max ph: 22
const nana::paint::font r11("", 11); // max ph: 24
const nana::paint::font r12("", 12); // max ph: 27
const nana::paint::font r14("", 14); // max ph: 32
const nana::paint::font r16("", 16); // max ph: 36
const nana::paint::font r18("", 18); // max ph: 41
const nana::paint::font r20("", 20); // max ph: 46
const nana::paint::font r22("", 22); // max ph: 51
const nana::paint::font r24("", 24); // max ph: 56
*/

//const nana::paint::font karla10(10, "assets/Karla-Regular.ttf");
//const nana::paint::font karla12(12, "assets/Karla-Regular.ttf");
//const nana::paint::font karla14(14, "assets/Karla-Regular.ttf");
//const nana::paint::font karla16(16, "assets/Karla-Regular.ttf");

inline const nana::paint::font& xs() { return util::sc->hd ? ui::fonts::r10 : ui::fonts::r8; };
inline const nana::paint::font& sm() { return util::sc->hd ? ui::fonts::r12 : ui::fonts::r10; };
inline const nana::paint::font& md() { return util::sc->hd ? ui::fonts::r14 : ui::fonts::r12; };
inline const nana::paint::font& lg() { return util::sc->hd ? ui::fonts::r16 : ui::fonts::r14; };
inline const nana::paint::font& xl() { return util::sc->hd ? ui::fonts::r18 : ui::fonts::r16; };

} // fonts
