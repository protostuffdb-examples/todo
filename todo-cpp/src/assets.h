#pragma once

#include <nana/gui/wvl.hpp>

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

namespace w$ {

#ifdef WIN32
const char* const label8 = "margin=[0,5]<_>";
const char* const label9 = label8;
const char* const label10 = label8;
const char* const label11 = label8;
const char* const label12 = label8;
const char* const label14 = label8;
const char* const label16 = label8;
const char* const label18 = label8;
const char* const label20 = label8;
const char* const label22 = label8;
const char* const label24 = label8;
#else
const char* const label8 = "margin=[1,5,0,5]<_>";
const char* const label9 = label8;
const char* const label10 = "margin=[2,5,0,5]<_>";
const char* const label11 = label10;
const char* const label12 = "margin=[3,5,0,5]<_>";
const char* const label14 = "margin=[4,5,0,5]<_>";
const char* const label16 = "margin=[5,5,0,5]<_>";
const char* const label18 = "margin=[6,5,0,5]<_>";
const char* const label20 = "margin=[7,5,0,5]<_>";
const char* const label22 = "margin=[8,5,0,5]<_>";
const char* const label24 = "margin=[9,5,0,5]<_>";
#endif

} // w$

namespace fonts {

const nana::paint::font r8("", 8);
const nana::paint::font r9("", 9);
const nana::paint::font r10("", 10);
const nana::paint::font r11("", 11);
const nana::paint::font r12("", 12);
const nana::paint::font r14("", 14);
const nana::paint::font r16("", 16);
const nana::paint::font r18("", 18);
const nana::paint::font r20("", 20);
const nana::paint::font r22("", 22);
const nana::paint::font r24("", 24);

//const nana::paint::font karla10(10, "assets/Karla-Regular.ttf");
//const nana::paint::font karla12(12, "assets/Karla-Regular.ttf");
//const nana::paint::font karla14(14, "assets/Karla-Regular.ttf");
//const nana::paint::font karla16(16, "assets/Karla-Regular.ttf");

} // fonts

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

struct ScreenConfig
{
    const int height;
    const int lb_height;
    const bool hd;
    ScreenConfig(int height):
        height(height),
        lb_height(height - LB_OUTER),
        hd(height >= MAX_HEIGHT)
    {
        
    }
};

ScreenConfig* sc{ nullptr };

} // util
