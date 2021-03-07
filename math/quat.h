#pragma once

#include "vector.h"
#include "math_utils.h"

namespace sprint {

struct Matrix;

class SERIALIZED Quat {
public:
    Quat() noexcept = default;
    Quat(float x, float y, float z, float w) noexcept : x_(x), y_(y), z_(z), w_(w) {}
    Quat(const Vec3& axis, float angle_rad);
    Quat(const Vec3& right, const Vec3& up, const Vec3& forward);
    explicit Quat(const Matrix&);

    explicit operator Matrix() const;

    Vec3 operator*(const Vec3& rhs) const;

    Quat operator*(const Quat& rhs) const;
    Quat& operator*=(const Quat& rhs);

    Quat& Normalize();

    [[nodiscard]] vec3 ToEuler() const;
    void FromEuler(const vec3&);

    [[nodiscard]] bool IsNaN() const {
        return math::IsNaN(w_) || math::IsNaN(x_) || math::IsNaN(y_) || math::IsNaN(z_);
    }

    static Quat Euler(const vec3&);
    static Quat Angle(const Vec3& from, const Vec3& to);
    static Quat LookAt(const Vec3& direction, const Vec3& up);
    static Quat Inverse(const Quat& q);
    static const Quat Identity;
    
private:
    SERIALIZABLE float x_ = 0.0f;
    SERIALIZABLE float y_ = 0.0f;
    SERIALIZABLE float z_ = 0.0f;
    SERIALIZABLE float w_ = 1.0f;
};

using quat = Quat;


}