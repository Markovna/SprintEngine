#pragma once

#include "vector.h"

namespace sprint {

struct Matrix;

struct SERIALIZED Quat {
    float x = 0, y = 0, z = 0, w = 1;

    Quat() noexcept {}
    Quat(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}
    Quat(const Vec3& axis, float angle_rad);
    Quat(const Matrix&);

    Vec3 operator*(const Vec3& rhs) const;

    Quat operator*(const Quat& rhs) const;
    Quat& operator*=(const Quat& rhs);

    Quat& Normalize();

    static Quat Inverse(const Quat& q);
    static const Quat Identity;
};

using quat = Quat;


}