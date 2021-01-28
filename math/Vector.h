#pragma once

#include <string>
#include <iomanip>
#include "spdlog/fmt/ostr.h"

namespace sprint {

template <size_t N> struct Vector;


using vec2 = Vector<2>;
using vec3 = Vector<3>;
using vec4 = Vector<4>;

using Vec2 = Vector<2>;
using Vec3 = Vector<3>;
using Vec4 = Vector<4>;

template <size_t N>
struct Vector {

    const float& operator[] (size_t index) const {
        assert(index >= 0 && index < N);
        return data_[index];
    }

    float& operator[] (size_t index) {
        assert(index >= 0 && index < N);
        return data_[index];
    }

private:
    float data_[N];
};

template<>
struct Vector<2> {
    float x, y;
    static constexpr const size_t N = 2;

    static const Vector Zero;
    static const Vector One;

    Vector() : x(0), y(0) {}
    explicit Vector(float x, float y) : x(x), y(y) {
    }

    const float& operator[] (size_t index) const {
        assert(index >= 0 && index < N);
        switch (index) {
            default:
            case 0: return x;
            case 1: return y;
        }
    }

    float& operator[] (size_t index) {
        assert(index >= 0 && index < N);
        switch (index) {
            default:
            case 0: return x;
            case 1: return y;
        }
    }
};

template<>
struct Vector<3> {
    float x, y, z;
    static constexpr const size_t N = 3;

    static const Vector Zero;
    static const Vector One;
    static const Vector Up;
    static const Vector Down;
    static const Vector Forward;
    static const Vector Backward;
    static const Vector Right;
    static const Vector Left;

    explicit Vector(const Vec2& vec, float z = 0.0f) noexcept : x(vec.x), y(vec.y), z(z) {}
    Vector() noexcept : x(0), y(0), z(0) {}
    Vector(float x, float y, float z) noexcept : x(x), y(y), z(z) {}

    explicit operator Vec2() const { return Vec2(x, y); }

    const float& operator[] (size_t index) const {
        assert(index >= 0 && index < N);
        switch (index) {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
    }

    float& operator[] (size_t index) {
        assert(index >= 0 && index < N);
        switch (index) {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
        }
    }

    Vector operator^(const Vector& rhs) const {
        return Vector(
                y * rhs.z - z * rhs.y,
                z * rhs.x - x * rhs.z,
                x * rhs.y - y * rhs.x
            );
    }

    float operator|(const Vector<3>& rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    [[nodiscard]] float Length() const {
        return std::sqrt(*this | *this);
    }
};

template<>
struct Vector<4> {
    float x, y, z, w;
    static constexpr const size_t N = 4;

    explicit Vector(const Vec3& vec, float w = 0.0f) : x(vec.x), y(vec.y), z(vec.z), w(w) {}
    Vector() : x(0), y(0), z(0), w(0) {}
    Vector(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {
    }

    explicit operator Vec2() const { return Vec2(x, y); }
    explicit operator Vec3() const { return Vec3(x, y, z); }

    const float& operator[] (size_t index) const {
        assert(index >= 0 && index < N);
        switch (index) {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }
    }

    float& operator[] (size_t index) {
        assert(index >= 0 && index < N);
        switch (index) {
            default:
            case 0: return x;
            case 1: return y;
            case 2: return z;
            case 3: return w;
        }
    }
};

template<typename OStream, size_t N>
OStream &operator<<(OStream &out, const Vector<N>& vec) {
    out << '[';
    for (size_t i = 0; i < N - 1; i++) {
        out << std::setprecision(5) << vec[i] << ", ";
    }
    return out << std::setprecision(5) << vec[N - 1] << ']';
}

template<size_t N>
Vector<N> operator+(const Vector<N>& lhs, const Vector<N>& rhs) {
    Vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] + rhs[i];
    }
    return res;
}

template<size_t N>
Vector<N> operator-(const Vector<N>& lhs, const Vector<N>& rhs) {
    Vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] - rhs[i];
    }
    return res;
}

template<size_t N>
Vector<N> operator-(const Vector<N>& rhs) {
    Vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = -rhs[i];
    }
    return res;
}

template<size_t N>
Vector<N> operator*(const Vector<N>& lhs, const Vector<N>& rhs) {
    Vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] * rhs[i];
    }
    return res;
}

template<size_t N>
Vector<N> operator*(float lhs, const Vector<N>& rhs) {
    Vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs * rhs[i];
    }
    return res;
}

template<size_t N>
Vector<N> operator*(const Vector<N>& lhs, float rhs) {
    Vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] * rhs;
    }
    return res;
}

template<size_t N>
Vector<N> operator/(const Vector<N>& lhs, float rhs) {
    Vector<N> res;
    const float scale = 1.0f / rhs;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] * scale;
    }
    return res;
}

template<size_t N>
Vector<N> operator/(float rhs, const Vector<N>& lhs) {
    Vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = rhs / lhs[i];
    }
    return res;
}

template<size_t N>
float operator|(const Vector<N>& lhs, const Vector<N>& rhs) {
    float res = 0;
    for (int i = 0; i < N; ++i) {
        res += lhs[i] * rhs[i];
    }
    return res;
}

template<size_t N>
Vector<N>& operator+=(Vector<N>& lhs, const Vector<N>& rhs) {
    for (int i = 0; i < N; ++i) {
        lhs[i] += rhs[i];
    }
    return lhs;
}

template<size_t N>
Vector<N>& operator-=(Vector<N>& lhs, const Vector<N>& rhs) {
    for (int i = 0; i < N; ++i) {
        lhs[i] -= rhs[i];
    }
    return lhs;
}

template<size_t N>
Vector<N>& operator*=(Vector<N>& lhs, const Vector<N>& rhs) {
    for (int i = 0; i < N; ++i) {
        lhs[i] *= rhs[i];
    }
    return lhs;
}

template<size_t N>
Vector<N>& operator/=(Vector<N>& lhs, float rhs) {
    const float scale = 1.0f / rhs;
    for (int i = 0; i < N; ++i) {
        lhs[i] *= scale;
    }
    return lhs;
}

template<size_t N>
bool operator==(const Vector<N>& lhs, const Vector<N>& rhs) {
    bool eq = true;
    for (int i = 0; i < N; ++i) {
        eq &= lhs[i] == rhs[i];
    }
    return eq;
}

template<size_t N>
bool operator!=(const Vector<N>& lhs, const Vector<N>& rhs) {
    return !(lhs == rhs);
}

template<class Vec>
Vec Normalized(const Vec& vec) {
    return vec / std::sqrt(vec | vec);
}

struct Vec2Int {
    int x{}, y{};
};

inline bool operator==(const Vec2Int& lhs, const Vec2Int& rhs) {
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(const Vec2Int& lhs, const Vec2Int& rhs) {
    return !(lhs == rhs);
}

template<typename OStream>
OStream &operator<<(OStream &out, const Vec2Int& vec) {
    return out << '[' << vec.x << ", " << vec.y << ']';
}

}