#pragma once

#include <cstdint>

namespace sprint {

struct Rect {
    int x, y;
    uint32_t width, height;
};


inline bool Valid(const Rect& rect) { return rect.width > 0 && rect.height > 0; }

}
