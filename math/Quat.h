#pragma once

#include "Vector.h"

struct Quat {
    float X, Y, Z, W;

    Quat();
    Quat(float x, float y, float z, float w);
    Quat(const Vec3& Axis, float AngleRad);

    inline Vec3 operator*(const Vec3& vec) const {
        const Vec3 q(X, Y, Z);
        const Vec3 t = 2.f * (q ^ vec);
        return vec + (W * t) + (q ^ t);
    }

    static const Quat Identity;
};


