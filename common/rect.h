#pragma once

namespace sprint {

struct Rect {
    int x{}, y{};
    uint32_t width{}, height{};
};

inline bool Valid(const Rect& rect) { return rect.width > 0 && rect.height > 0; }

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
