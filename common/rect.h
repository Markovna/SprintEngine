#pragma once
#include "meta_runtime.h"

namespace sprint {

class SERIALIZED Rect {
public:
    Rect() noexcept = default;
    Rect(float _x, float _y, float _width, float _height) noexcept
        : x(_x), y(_y), width(_width), height(_height)
    {}

public:
    float x = 0;
    float y = 0;
    float width = 1;
    float height = 1;
};

inline bool operator==(const Rect& lhs, const Rect& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y &&
        lhs.width == rhs.width && lhs.height == rhs.height;
}

inline bool operator!=(const Rect& lhs, const Rect& rhs) {
    return !(lhs == rhs);
}

template<typename OStream>
OStream &operator<<(OStream &out, const Rect& rect) {
    return out << '[' << rect.x << ", " << rect.y
               << ", " << rect.width << ", " << rect.height << ']';
}

}