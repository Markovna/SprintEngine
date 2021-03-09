#include "color.h"


namespace sprint {

const Color Color::Black(0.0f, 0.0f, 0.0f, 1.0f);
const Color Color::White(1.0f, 1.0f, 1.0f, 1.0f);
const Color Color::Red(1.0f, 0.0f, 0.0f, 1.0f);
const Color Color::Green(0.0f, 1.0f, 0.0f, 1.0f);
const Color Color::Blue(0.0f, 0.0f, 1.0f, 1.0f);

Color::Color(float r, float g, float b, float a) noexcept : r(r), g(g), b(b), a(a) {}
Color::Color() noexcept : r(), g(), b(), a() {}

bool Color::operator!=(const Color &other) const {
    return !(*this == other);
}

bool Color::operator==(const Color &other) const {
    return r == other.r && g == other.g && b == other.b && a == other.a;
}

void Color::operator-=(const Color &other) {
    r -= other.r; g -= other.g; b -= other.b; a -= other.a;
}

Color Color::operator-(const Color &rhs) const {
    return Color(r - rhs.r, g - rhs.g, b - rhs.b, a - rhs.a);
}

void Color::operator+=(const Color &other) {
    r += other.r; g += other.g; b += other.b; a += other.a;
}

Color Color::operator+(const Color &rhs) const {
    return Color(r + rhs.r, g + rhs.g, b + rhs.b, a + rhs.a);
}

void Color::operator*=(const Color &other) {
    r *= other.r; g *= other.g; b *= other.b; a *= other.a;
}

Color Color::operator*(const Color &rhs) const {
    return Color(r * rhs.r, g * rhs.g, b * rhs.b, a * rhs.a);
}

}