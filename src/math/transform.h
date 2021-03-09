#pragma once

#include "vector.h"
#include "quat.h"
#include "matrix.h"
#include "reflection/meta_runtime.h"

namespace sprint {

class SERIALIZED Transform {
public:
    Transform() noexcept
        : position_(), rotation_(), scale_(Vec3::One) {}

    Transform(const vec3& p, const quat& r, const vec3& s) noexcept
        : position_(p), rotation_(r), scale_(s) {}

    explicit Transform(const mat4& matrix) {
        SetFromMatrix(matrix);
    }

    Transform(const Transform& other) = default;
    Transform& operator=(const Transform& other) = default;

    [[nodiscard]] vec3 TransformCoord(const Vec3& value) const {
        return position_ + (rotation_ * value) * scale_;
    }

    [[nodiscard]] vec3 TransformVector(const Vec3& value) const {
        return (rotation_ * value) * scale_;
    }

    [[nodiscard]] vec3 TransformDirection(const Vec3& value) const {
        return rotation_ * value;
    }

    [[nodiscard]] vec3 Up() const {
        return TransformDirection(vec3::Up);
    }

    [[nodiscard]] vec3 Forward() const {
        return TransformDirection(vec3::Forward);
    }

    [[nodiscard]] vec3 Right() const {
        return TransformDirection(vec3::Right);
    }

    Transform operator*(const Transform& rhs) const {
        return { rotation_ * (rhs.position_ * scale_) + position_, rotation_ * rhs.rotation_, scale_ * rhs.scale_};
    }

    void SetFromMatrix(const mat4& matrix) {
        mat4 tmp = matrix;

        scale_ = tmp.NormalizeScale();
        if (matrix.Determinant() < 0.f) {
            scale_.x *= -1.f;
            tmp.SetRow(0, -tmp.GetRow(0));
        }

        position_ = matrix.GetOrigin();
        rotation_ = Quat(tmp).Normalize();
    }

    explicit operator mat4() const {
        return mat4::TRS(position_, rotation_, scale_);
    }

    static Transform Inverse(const Transform& transform) {
        quat rotation = quat::Inverse(transform.rotation_);
        vec3 scale = 1 / transform.scale_;
        vec3 position = rotation * (scale * -transform.position_);
        return Transform(position, rotation, scale);
    }

    [[nodiscard]] const vec3& get_position() const { return position_; }
    [[nodiscard]] const vec3& get_scale() const { return scale_; }
    [[nodiscard]] const quat& get_rotation() const { return rotation_; }

    void set_position(const vec3& pos) { position_ = pos; }
    void set_scale(const vec3& scale) { scale_ = scale; }
    void set_rotation(const quat& rot) { rotation_ = rot; }
private:
    SERIALIZABLE vec3 position_;
    SERIALIZABLE quat rotation_;
    SERIALIZABLE vec3 scale_;
};

}


