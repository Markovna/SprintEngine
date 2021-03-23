#pragma once

#include <numeric>

#include "math/math_utils.h"
#include "math/matrix.h"
#include "renderer/gfx.h"
#include "common/rect.h"
#include "common/int_set.h"
#include "reflection/meta_runtime.h"

namespace sprint {

class SERIALIZED Camera {
private:
    using ClearFlag = gfx::ClearFlag::Type;

private:
    static int_set<uint32_t, gfx::static_config::kCamerasCapacity>& Registry();

    static uint32_t NextID() {
        return Registry().next();
    }

    static void FreeID(uint32_t id) {
        Registry().free(id);
    }

public:
    enum Kind { Game, Editor };

public:
    explicit Camera(Kind kind = Game) : idx_(NextID()), kind_(kind) {}
    ~Camera();

    [[nodiscard]] uint32_t CameraID() const { return idx_; }

    [[nodiscard]] float GetFieldOfView() const { return fov_; }
    [[nodiscard]] float GetNear() const { return near_; }
    [[nodiscard]] float GetFar() const { return far_; }
    [[nodiscard]] float GetOrthoSize() const { return orthogonal_size_; }
    [[nodiscard]] Rect GetRect() const { return normalized_rect_; }
    [[nodiscard]] Color GetClearColor() const { return clear_color_; }
    [[nodiscard]] ClearFlag GetClearFlag() const { return clear_flag_; }
    [[nodiscard]] Kind GetKind() const { return kind_; }

    void SetClearColor(const Color& color) { clear_color_ = color; }
    void SetFieldOfView(float fov) { fov_ = fov; }
    void SetNear(float near) { near_ = near; }
    void SetFar(float far) { far_ = far; }
    void SetRect(const Rect& rect) { normalized_rect_ = rect; }

private:
    uint32_t idx_;
    SERIALIZABLE float fov_ = 60.0f;
    SERIALIZABLE float near_ = 0.1f;
    SERIALIZABLE float far_ = 100.0f;
    SERIALIZABLE float orthogonal_size_ = 1.0f;
    SERIALIZABLE Rect normalized_rect_ = {0.0f, 0.0f, 1.0f, 1.0f};
    SERIALIZABLE Color clear_color_ = Color::Black;
    SERIALIZABLE ClearFlag clear_flag_ = gfx::ClearFlag::Color | gfx::ClearFlag::Depth;
    Kind kind_;
};

}