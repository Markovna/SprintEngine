#include "Color.h"


namespace Sprint {

const Color Color::Black(0.0f, 0.0f, 0.0f, 1.0f);
const Color Color::White(1.0f, 1.0f, 1.0f, 1.0f);
const Color Color::Red(1.0f, 0.0f, 0.0f, 1.0f);
const Color Color::Green(0.0f, 1.0f, 0.0f, 1.0f);
const Color Color::Blue(0.0f, 0.0f, 1.0f, 1.0f);

Color::Color(float r, float g, float b, float a) noexcept : R(r), G(g), B(b), A(a) {}
Color::Color() noexcept : R(), G(), B(), A() {}

bool Color::operator!=(const Color &other) const {
    return !(*this == other);
}

bool Color::operator==(const Color &other) const {
    return R == other.R && G == other.G && B == other.B && A == other.A;
}

void Color::operator-=(const Color &other) {
    R -= other.R; G -= other.G; B -= other.B; A -= other.A;
}

Color Color::operator-(const Color &rhs) const {
    return Color(R - rhs.R, G - rhs.G,B - rhs.B,A - rhs.A);
}

void Color::operator+=(const Color &other) {
    R += other.R; G += other.G; B += other.B; A += other.A;
}

Color Color::operator+(const Color &rhs) const {
    return Color(R + rhs.R, G + rhs.G,B + rhs.B,A + rhs.A);
}

void Color::operator*=(const Color &other) {
    R *= other.R; G *= other.G; B *= other.B; A *= other.A;
}

Color Color::operator*(const Color &rhs) const {
    return Color(R * rhs.R, G * rhs.G,B * rhs.B,A * rhs.A);
}

}