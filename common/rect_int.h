#pragma once
#include "meta_runtime.h"

namespace sprint {

struct SERIALIZED RectInt {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

inline bool Valid(const RectInt& rect) { return rect.width > 0 && rect.height > 0; }

inline bool operator==(const RectInt& lhs, const RectInt& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y &&
            lhs.width == rhs.width && lhs.height == rhs.height;
}

inline bool operator!=(const RectInt& lhs, const RectInt& rhs) {
    return !(lhs == rhs);
}

template<typename OStream>
OStream &operator<<(OStream &out, const RectInt& rect) {
    return out << '[' << rect.x << ", " << rect.y
                << ", " << rect.width << ", " << rect.height << ']';
}

}
