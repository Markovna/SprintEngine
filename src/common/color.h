#pragma once
#include "reflection/meta_runtime.h"

namespace sprint {

struct SERIALIZED Color {

    static const Color Black;
    static const Color White;
    static const Color Red;
    static const Color Green;
    static const Color Blue;

    Color(float r, float g, float b, float a = 1.0f) noexcept;
    Color() noexcept;

    float r;
    float g;
    float b;
    float a;

    Color operator*(const Color& rhs) const;
    void operator*=(const Color& other);

    Color operator+(const Color& rhs) const;
    void operator+=(const Color& other);

    Color operator-(const Color& rhs) const;
    void operator-=(const Color& other);

    bool operator==(const Color& other) const;
    bool operator!=(const Color& other) const;
};

}