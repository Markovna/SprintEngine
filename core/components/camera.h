#pragma once

#include <numeric>

#include "math_utils.h"
#include "matrix.h"
#include "gfx.h"
#include "int_set.h"
#include "meta_runtime.h"

namespace sprint {

class SERIALIZED Camera {
private:
    using ClearFlag = gfx::ClearFlag::Type;

private:
    static int_set<uint32_t, gfx::static_config::kCamerasCapacity> idx_registry_;

    static uint32_t NextID() {
        return idx_registry_.next();
    }

    static void FreeID(uint32_t id) {
        idx_registry_.free(id);
    }

public:
    explicit Camera() : idx_(NextID()) {}
    ~Camera();

    [[nodiscard]] uint32_t CameraID() const { return idx_; }

    [[nodiscard]] Matrix GetProjectionMatrix() const;
    [[nodiscard]] float GetFieldOfView() const { return fov_; }
    [[nodiscard]] float GetNear() const { return near_; }
    [[nodiscard]] float GetFar() const { return far_; }
    [[nodiscard]] vec2 GetRectSize() const { return rect_size_; }

    void SetFieldOfView(float fov) { fov_ = fov; }
    void SetNear(float near) { near_ = near; }
    void SetFar(float far) { far_ = far; }
    void SetRectSize(const vec2& size) { rect_size_ = size; }

private:
    const uint32_t idx_;
    SERIALIZABLE float fov_ = 60.0f;
    SERIALIZABLE float near_ = 0.1f;
    SERIALIZABLE float far_ = 100.0f;
    SERIALIZABLE vec2 rect_size_ = vec2::One;
    SERIALIZABLE Color clear_color_ = Color::Black;
    SERIALIZABLE ClearFlag clear_flag_ = gfx::ClearFlag::Color | gfx::ClearFlag::Depth;
};

}