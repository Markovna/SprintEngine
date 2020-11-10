#pragma once

namespace sprint {

struct Color {

    static const Color Black;
    static const Color White;
    static const Color Red;
    static const Color Green;
    static const Color Blue;

    Color(float r, float g, float b, float a = 1.0f) noexcept;
    Color() noexcept;

    float r, g, b, a;

    inline Color operator*(const Color& rhs) const;
    inline void operator*=(const Color& other);

    inline Color operator+(const Color& rhs) const;
    inline void operator+=(const Color& other);

    inline Color operator-(const Color& rhs) const;
    inline void operator-=(const Color& other);

    inline bool operator==(const Color& other) const;
    inline bool operator!=(const Color& other) const;
};

}