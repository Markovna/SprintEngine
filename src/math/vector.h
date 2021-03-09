#pragma once

#include <string>
#include <iomanip>
#include "spdlog/fmt/ostr.h"
#include "reflection/meta_runtime.h"

namespace sprint {

template <size_t N> struct vector;


using vec2 = vector<2>;
using vec3 = vector<3>;
using vec4 = vector<4>;

using Vec2 = vector<2>;
using Vec3 = vector<3>;
using Vec4 = vector<4>;

template <size_t N>
struct SERIALIZED vector {

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
struct SERIALIZED vector<2> {
    float x, y;
    static constexpr const size_t N = 2;

    static const vector Zero;
    static const vector One;

    vector() : x(0), y(0) {}
    vector(float x, float y) : x(x), y(y) {}

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
struct SERIALIZED vector<3> {
    float x, y, z;
    static constexpr const size_t N = 3;

    static const vector Zero;
    static const vector One;
    static const vector Forward;
    static const vector Backward;
    static const vector Right;
    static const vector Left;
    static const vector Up;
    static const vector Down;

    explicit vector(const Vec2& vec, float z = 0.0f) noexcept : x(vec.x), y(vec.y), z(z) {}
    vector() noexcept : x(0), y(0), z(0) {}
    vector(float x, float y, float z) noexcept : x(x), y(y), z(z) {}

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

    vector operator^(const vector& rhs) const {
        return vector(
                y * rhs.z - z * rhs.y,
                z * rhs.x - x * rhs.z,
                x * rhs.y - y * rhs.x
            );
    }

    float operator|(const vector<3>& rhs) const {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    [[nodiscard]] float Length() const {
        return std::sqrtf(*this | *this);
    }

    [[nodiscard]] float SqrLength() const {
        return *this | *this;
    }

    vec3& Normalize() {
        float inv_l = 1.0f / Length();
        x *= inv_l, y *= inv_l, z *= inv_l;
        return *this;
    }
};

template<>
struct vector<4> {
    float x, y, z, w;
    static constexpr const size_t N = 4;

    explicit vector(const Vec3& vec, float w = 0.0f) : x(vec.x), y(vec.y), z(vec.z), w(w) {}
    vector() : x(0), y(0), z(0), w(0) {}
    vector(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {
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
OStream &operator<<(OStream &out, const vector<N>& vec) {
    out << '[';
    for (size_t i = 0; i < N - 1; i++) {
        out << std::setprecision(5) << vec[i] << ", ";
    }
    return out << std::setprecision(5) << vec[N - 1] << ']';
}

template<size_t N>
vector<N> operator+(const vector<N>& lhs, const vector<N>& rhs) {
    vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] + rhs[i];
    }
    return res;
}

template<size_t N>
vector<N> operator-(const vector<N>& lhs, const vector<N>& rhs) {
    vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] - rhs[i];
    }
    return res;
}

template<size_t N>
vector<N> operator-(const vector<N>& rhs) {
    vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = -rhs[i];
    }
    return res;
}

template<size_t N>
vector<N> operator*(const vector<N>& lhs, const vector<N>& rhs) {
    vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] * rhs[i];
    }
    return res;
}

template<size_t N>
vector<N> operator*(float lhs, const vector<N>& rhs) {
    vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs * rhs[i];
    }
    return res;
}

template<size_t N>
vector<N> operator*(const vector<N>& lhs, float rhs) {
    vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] * rhs;
    }
    return res;
}

template<size_t N>
vector<N> operator/(const vector<N>& lhs, float rhs) {
    vector<N> res;
    const float scale = 1.0f / rhs;
    for (int i = 0; i < N; ++i) {
        res[i] = lhs[i] * scale;
    }
    return res;
}

template<size_t N>
vector<N> operator/(float rhs, const vector<N>& lhs) {
    vector<N> res;
    for (int i = 0; i < N; ++i) {
        res[i] = rhs / lhs[i];
    }
    return res;
}

template<size_t N>
float operator|(const vector<N>& lhs, const vector<N>& rhs) {
    float res = 0;
    for (int i = 0; i < N; ++i) {
        res += lhs[i] * rhs[i];
    }
    return res;
}

template<size_t N>
vector<N>& operator+=(vector<N>& lhs, const vector<N>& rhs) {
    for (int i = 0; i < N; ++i) {
        lhs[i] += rhs[i];
    }
    return lhs;
}

template<size_t N>
vector<N>& operator-=(vector<N>& lhs, const vector<N>& rhs) {
    for (int i = 0; i < N; ++i) {
        lhs[i] -= rhs[i];
    }
    return lhs;
}

template<size_t N>
vector<N>& operator*=(vector<N>& lhs, const vector<N>& rhs) {
    for (int i = 0; i < N; ++i) {
        lhs[i] *= rhs[i];
    }
    return lhs;
}

template<size_t N>
vector<N>& operator/=(vector<N>& lhs, float rhs) {
    const float scale = 1.0f / rhs;
    for (int i = 0; i < N; ++i) {
        lhs[i] *= scale;
    }
    return lhs;
}

template<size_t N>
bool operator==(const vector<N>& lhs, const vector<N>& rhs) {
    bool eq = true;
    for (int i = 0; i < N; ++i) {
        eq &= lhs[i] == rhs[i];
    }
    return eq;
}

template<size_t N>
bool operator!=(const vector<N>& lhs, const vector<N>& rhs) {
    return !(lhs == rhs);
}

template<class Vec>
Vec Normalized(const Vec& vec) {
    return vec / std::sqrt(vec | vec);
}

inline float Angle(const Vec3& lhs, const Vec3& rhs) {
    float len = lhs.Length() * rhs.Length();
    return std::acosf((lhs | rhs) / len);
}

struct Vec2Int {
    int x = 0;
    int y = 0;

    Vec2Int() noexcept = default;
    Vec2Int(int _x, int _y) noexcept : x(_x), y(_y) {}
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