#pragma once

#include <cstdio>
#include <variant>

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

template<>
struct Vector<2> {
    float X, Y;
    static constexpr size_t N = 2;

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

    explicit Vector(float x, float y, float z) : X(x), Y(y), Z(z) {
    }

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
};

template<>
struct Vector<4> {
    float X, Y, Z, W;
    static constexpr size_t N = 4;

    explicit Vector(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {
    }

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

typedef Vector<2> Vec2;
typedef Vector<3> Vec3;
typedef Vector<4> Vec4;