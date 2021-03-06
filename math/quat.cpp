#include "quat.h"
#include "matrix.h"
#include "math_utils.h"

namespace sprint {

const Quat Quat::Identity(0.0f,0.0f,0.0f,1.0f);

Quat::Quat(const Vec3& axis, float angle_rad) {
    const float halfAngle = 0.5f * angle_rad;
    const float s = std::sinf(halfAngle);
    x = s * axis.x; y = s * axis.y; z = s * axis.z; w = std::cosf(halfAngle);
}

Quat::Quat(const mat4& mat) {
    float s;
    const float tr = mat[0][0] + mat[1][1] + mat[2][2];

    if (tr > 0.0f) {
        float t = tr + 1.0f;
        s = 1.0f / std::sqrtf(t) * 0.5f;

        x = (mat[1][2] - mat[2][1]) * s;
        y = (mat[2][0] - mat[0][2]) * s;
        z = (mat[0][1] - mat[1][0]) * s;
        w = s * t;
    }
    else {
        int32_t i = 0;

        if (mat[1][1] > mat[0][0]) i = 1;
        if (mat[2][2] > mat[i][i]) i = 2;

        static const int32_t next[3] = { 1, 2, 0 };
        const int32_t j = next[i];
        const int32_t k = next[j];

        s = mat[i][i] - mat[j][j] - mat[k][k] + 1.0f;

        float sqrt_s = std::sqrtf(s);

        float qt[4];
        qt[i] = 0.5f * sqrt_s;

        s = 0.5f / sqrt_s;

        qt[3] = (mat[j][k] - mat[k][j]) * s;
        qt[j] = (mat[i][j] + mat[j][i]) * s;
        qt[k] = (mat[i][k] + mat[k][i]) * s;

        x = qt[0]; y = qt[1]; z = qt[2]; w = qt[3];
    }
}

Quat& Quat::Normalize() {
    float l = 1.0f / std::sqrtf(x * x + y * y + z * z + w * w);
    x *= l; y *= l; z *= l; w *= l;
    return *this;
}

Quat Quat::Inverse(const Quat &q) {
    return Quat(-q.x, -q.y, -q.z, q.w);
}

Quat Quat::operator*(const Quat &rhs) const {
    return Quat(w * rhs.x + rhs.w * x + y * rhs.z - rhs.y * z,
                w * rhs.y + rhs.w * y + z * rhs.x - rhs.z * x,
                w * rhs.z + rhs.w * z + x * rhs.y - rhs.x * y,
                w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z);
}

Quat &Quat::operator*=(const Quat &rhs) {
    *this = *this * rhs;
    return *this;
}

Vec3 Quat::operator*(const Vec3 &rhs) const {
    const Vec3 q(x, y, z);
    const Vec3 t = 2.f * (q ^ rhs);
    return rhs + (w * t) + (q ^ t);
}

vec3 Quat::ToEuler() const {
    float check = 2.0f * (w * x - y * z);

    if (check < -0.999999f) {
        return Vec3(-
            90.0f,
            0.0f,
            math::TO_DEGREE * -std::atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)));
    }

    if (check > 0.999999f) {
        return Vec3(
            90.0f,
            0.0f,
            math::TO_DEGREE * std::atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)));
    }

    return Vec3(
        math::TO_DEGREE * std::asinf(check),
        math::TO_DEGREE * std::atan2f(2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + y * y)),
        math::TO_DEGREE * std::atan2f(2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z)));
}

void Quat::FromEuler(const vec3& euler) {
    assert(euler.x >= -90.0f && euler.x <= 90.0f);
    float ex = euler.x * 0.5f * math::TO_RAD;
    float ey = euler.y * 0.5f * math::TO_RAD;
    float ez = euler.z * 0.5f * math::TO_RAD;
    float sinX = std::sinf(ex);
    float cosX = std::cosf(ex);
    float sinY = std::sinf(ey);
    float cosY = std::cosf(ey);
    float sinZ = std::sinf(ez);
    float cosZ = std::cosf(ez);

    w = cosY * cosX * cosZ + sinY * sinX * sinZ;
    x = cosY * sinX * cosZ + sinY * cosX * sinZ;
    y = sinY * cosX * cosZ - cosY * sinX * sinZ;
    z = cosY * cosX * sinZ - sinY * sinX * cosZ;
}

}