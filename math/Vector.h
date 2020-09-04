#pragma once

#include <string>
#include <iomanip>
#include "spdlog/fmt/ostr.h"


namespace Sprint {

template <size_t N>
struct Vector {

    const float& operator[] (size_t index) const {
        assert(index >= 0 && index < N);
        return m_Data[index];
    }

    float& operator[] (size_t index) {
        assert(index >= 0 && index < N);
        return m_Data[index];
    }

private:
    float m_Data[N];
};

typedef Vector<2> Vec2;
typedef Vector<3> Vec3;
typedef Vector<4> Vec4;

template<>
struct Vector<2> {
    float X, Y;
    static constexpr size_t N = 2;

    static const Vector Zero;
    static const Vector One;

    Vector() : X(0), Y(0) {}
    explicit Vector(float x, float y) : X(x), Y(y) {
    }

    const float& operator[] (size_t index) const {
        assert(index >= 0 && index < N);
        switch (index) {
            default:
            case 0: return X;
            case 1: return Y;
        }
    }

    float& operator[] (size_t index) {
        assert(index >= 0 && index < N);
        switch (index) {
            default:
            case 0: return X;
            case 1: return Y;
        }
    }
};

template<>
struct Vector<3> {
    float X, Y, Z;
    static constexpr size_t N = 3;

    static const Vector Zero;
    static const Vector One;
    static const Vector Up;
    static const Vector Down;
    static const Vector Forward;
    static const Vector Backward;
    static const Vector Right;
    static const Vector Left;

    explicit Vector(const Vec2& vec, float z = 0.0f) : X(vec.X), Y(vec.Y), Z(z) {}
    Vector() : X(0), Y(0), Z(0) {}
    Vector(float x, float y, float z) : X(x), Y(y), Z(z) {
    }

    explicit operator Vec2() const { return Vec2(X, Y); }

    const float& operator[] (size_t index) const {
        assert(index >= 0 && index < N);
        switch (index) {
            default:
            case 0: return X;
            case 1: return Y;
            case 2: return Z;
        }
    }

    float& operator[] (size_t index) {
        assert(index >= 0 && index < N);
        switch (index) {
            default:
            case 0: return X;
            case 1: return Y;
            case 2: return Z;
        }
    }

    Vector operator^(const Vector& rhs) const {
        return Vector(
                Y * rhs.Z - Z * rhs.Y,
                Z * rhs.X - X * rhs.Z,
                X * rhs.Y - Y * rhs.X
            );
    }

    float operator|(const Vector<3>& rhs) const {
        return X * rhs.X + Y * rhs.Y + Z * rhs.Z;
    }
};

template<>
struct Vector<4> {
    float X, Y, Z, W;
    static constexpr size_t N = 4;

    explicit Vector(const Vec3& vec, float w = 0.0f) : X(vec.X), Y(vec.Y), Z(vec.Z), W(w) {}
    Vector() : X(0), Y(0), Z(0), W(0) {}
    Vector(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {
    }

    explicit operator Vec2() const { return Vec2(X, Y); }
    explicit operator Vec3() const { return Vec3(X, Y, Z); }

    const float& operator[] (size_t index) const {
        assert(index >= 0 && index < N);
        switch (index) {
            default:
            case 0: return X;
            case 1: return Y;
            case 2: return Z;
            case 3: return W;
        }
    }

    float& operator[] (size_t index) {
        assert(index >= 0 && index < N);
        switch (index) {
            default:
            case 0: return X;
            case 1: return Y;
            case 2: return Z;
            case 3: return W;
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

}