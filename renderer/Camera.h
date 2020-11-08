#pragma once

#include "Matrix.h"

namespace sprint {

class Camera {
public:
    explicit Camera(const Matrix& projection) :
            position_(Vec3::Zero),
            rotation_(Quat::Identity),
            projection_matrix_(projection)
    {}

    void set_rotation(const Quat& rotation) { rotation_ = rotation; }
    void set_position(const Vec3& position) { position_ = position; }
    void set_projection_matrix(const Matrix& mat) { projection_matrix_ = mat; }

    Matrix get_projection_matrix() const { return projection_matrix_; }
    Matrix GetViewMatrix() const { return Matrix::Translation(position_) * Matrix::Rotation(rotation_); }

private:
    Matrix projection_matrix_;

    Vec3 position_;
    Quat rotation_;
};

}

