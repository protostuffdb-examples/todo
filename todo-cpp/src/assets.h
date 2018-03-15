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

namespace fonts {

//const nana::paint::font karla10(10, "assets/Karla-Regular.ttf");
const nana::paint::font karla12(12, "assets/Karla-Regular.ttf");
//const nana::paint::font karla14(14, "assets/Karla-Regular.ttf");
//const nana::paint::font karla16(16, "assets/Karla-Regular.ttf");

} // fonts
