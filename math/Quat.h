#pragma once

#include "Vector.h"


namespace sprint {

struct Quat {
    float x, y, z, w;

    Quat();
    Quat(float x, float y, float z, float w);
    Quat(const Vec3& axis, float angle_rad);

    inline Vec3 operator*(const Vec3& vec) const {
        const Vec3 q(x, y, z);
        const Vec3 t = 2.f * (q ^ vec);
        return vec + (w * t) + (q ^ t);
    }

    static const Quat Identity;
};


}