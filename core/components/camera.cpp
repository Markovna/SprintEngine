#include "camera.h"

namespace sprint {

Matrix Camera::GetProjectionMatrix() const {
    if (fov_ > 0.0f) {
        return Matrix::Perspective(fov_ * math::PI / 180.0f, rect_size_.x, rect_size_.y, near_, far_);
    }

    return Matrix::Ortho(rect_size_.x, rect_size_.y, near_, far_);
}

Camera::~Camera() {
    FreeID(idx_);
}

int_set<uint32_t, gfx::static_config::kCamerasCapacity> Camera::idx_registry_{};

}