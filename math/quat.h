#pragma once

#include "vector.h"

namespace sprint {

struct Matrix;

struct SERIALIZED Quat {
    float x = 0, y = 0, z = 0, w = 1;

    Quat() noexcept = default;
    Quat(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}
    Quat(const Vec3& axis, float angle_rad);
    explicit Quat(const Matrix&);

    Vec3 operator*(const Vec3& rhs) const;

    Quat operator*(const Quat& rhs) const;
    Quat& operator*=(const Quat& rhs);

    Quat& Normalize();

    [[nodiscard]] vec3 ToEuler() const;
    void FromEuler(const vec3&);

    static Quat Inverse(const Quat& q);
    static const Quat Identity;
};

using quat = Quat;


}