#include "quat.h"
#include "matrix.h"
#include "math_utils.h"

namespace sprint {

const Quat Quat::Identity(0.0f,0.0f,0.0f,1.0f);

Quat::Quat(const Vec3& axis, float angle_rad) {
    const float halfAngle = 0.5f * angle_rad;
    const float s = std::sinf(halfAngle);
    x_ = s * axis.x; y_ = s * axis.y; z_ = s * axis.z; w_ = std::cosf(halfAngle);
}

Quat::Quat(const Vec3 &right, const Vec3 &up, const Vec3 &forward)
    : Quat(mat4(right, up, forward)) {}

Quat::Quat(const mat4& mat) {
    float s;
    const float tr = mat[0][0] + mat[1][1] + mat[2][2];

    if (tr > 0.0f) {
        float t = tr + 1.0f;
        s = 1.0f / std::sqrtf(t) * 0.5f;

        x_ = (mat[1][2] - mat[2][1]) * s;
        y_ = (mat[2][0] - mat[0][2]) * s;
        z_ = (mat[0][1] - mat[1][0]) * s;
        w_ = s * t;
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

        x_ = qt[0]; y_ = qt[1]; z_ = qt[2]; w_ = qt[3];
    }
}

Quat& Quat::Normalize() {
    float l = 1.0f / std::sqrtf(x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_);
    x_ *= l; y_ *= l; z_ *= l; w_ *= l;
    return *this;
}

Quat Quat::Inverse(const Quat &q) {
    return Quat(-q.x_, -q.y_, -q.z_, q.w_);
}

Quat Quat::operator*(const Quat &rhs) const {
    return Quat(w_ * rhs.x_ + rhs.w_ * x_ + y_ * rhs.z_ - rhs.y_ * z_,
                w_ * rhs.y_ + rhs.w_ * y_ + z_ * rhs.x_ - rhs.z_ * x_,
                w_ * rhs.z_ + rhs.w_ * z_ + x_ * rhs.y_ - rhs.x_ * y_,
                w_ * rhs.w_ - x_ * rhs.x_ - y_ * rhs.y_ - z_ * rhs.z_);
}

Quat &Quat::operator*=(const Quat &rhs) {
    *this = *this * rhs;
    return *this;
}

Vec3 Quat::operator*(const Vec3 &rhs) const {
    const Vec3 q(x_, y_, z_);
    const Vec3 t = 2.f * (q ^ rhs);
    return rhs + (w_ * t) + (q ^ t);
}

vec3 Quat::ToEuler() const {
    float check = 2.0f * (w_ * x_ - y_ * z_);

    if (check < -0.999999f) {
        return Vec3(
            -90.0f,
            0.0f,
            math::RAD_TO_DEG * -std::atan2f(2.0f * (x_ * z_ - w_ * y_), 1.0f - 2.0f * (y_ * y_ + z_ * z_)));
    }

    if (check > 0.999999f) {
        return Vec3(
            90.0f,
            0.0f,
            math::RAD_TO_DEG * std::atan2f(2.0f * (x_ * z_ - w_ * y_), 1.0f - 2.0f * (y_ * y_ + z_ * z_)));
    }

    return Vec3(
        math::RAD_TO_DEG * std::asinf(check),
        math::RAD_TO_DEG * std::atan2f(2.0f * (x_ * z_ + w_ * y_), 1.0f - 2.0f * (x_ * x_ + y_ * y_)),
        math::RAD_TO_DEG * std::atan2f(2.0f * (x_ * y_ + w_ * z_), 1.0f - 2.0f * (x_ * x_ + z_ * z_)));
}

void Quat::FromEuler(const vec3& euler) {
    float ex = euler.x * 0.5f * math::DEG_TO_RAD;
    float ey = euler.y * 0.5f * math::DEG_TO_RAD;
    float ez = euler.z * 0.5f * math::DEG_TO_RAD;

    float sinX = std::sinf(ex);
    float cosX = std::cosf(ex);
    float sinY = std::sinf(ey);
    float cosY = std::cosf(ey);
    float sinZ = std::sinf(ez);
    float cosZ = std::cosf(ez);

    w_ = cosY * cosX * cosZ + sinY * sinX * sinZ;
    x_ = cosY * sinX * cosZ + sinY * cosX * sinZ;
    y_ = sinY * cosX * cosZ - cosY * sinX * sinZ;
    z_ = cosY * cosX * sinZ - sinY * sinX * cosZ;
}

Quat Quat::LookAt(const Vec3 &direction, const Vec3 &up) {
    vec3 forward = Normalized(direction);

    vec3 v = forward ^ up;
    // if direction & up are parallel, fallback to Quat::Angle
    if (math::Approximately(v.SqrLength(), 0.0f)) {
        return Quat::Angle(vec3::Forward, forward);
    }

    v.Normalize();
    vec3 up_axis = v ^ forward;
    vec3 right_axis = up_axis ^ forward;
    return Quat(right_axis, up_axis, forward);
}

Quat Quat::Angle(const Vec3 &from, const Vec3 &to) {
    vec3 from_n = Normalized(from);
    vec3 to_n = Normalized(to);
    float dot = from_n | to_n;

    if (math::Approximately(dot, -1.0f)) {
        vec3 axis = vec3::Right ^ from_n;
        if (math::Approximately(0.0f, axis.Length()))
            axis = vec3::Up ^ from_n;

        return Quat(axis, math::PI);
    }

    vec3 c = from_n ^ to_n;
    float s = std::sqrtf((1.0f + dot) * 2.0f);
    float inv_s = 1.0f / s;
    return Quat(c.x * inv_s, c.y * inv_s, c.z * inv_s, 0.5f * s);
}

Quat::operator Matrix() const {
    const float x2 = x_ + x_;  const float y2 = y_ + y_;  const float z2 = z_ + z_;
    const float xx = x_ * x2; const float xy = x_ * y2; const float xz = x_ * z2;
    const float yy = y_ * y2; const float yz = y_ * z2; const float zz = z_ * z2;
    const float wx = w_ * x2; const float wy = w_ * y2; const float wz = w_ * z2;

    return Matrix(
        Vec3(1.0f - (yy + zz), xy + wz,          xz - wy),
        Vec3(xy - wz,          1.0f - (xx + zz), yz + wx),
        Vec3(xz + wy,          yz - wx,          1.0f - (xx + yy)),
        Vec3(0.0f,             0.0f,             0.0f)
    );
}

Quat Quat::Euler(const vec3& euler) {
    Quat ret;
    ret.FromEuler(euler);
    return ret;
}

}