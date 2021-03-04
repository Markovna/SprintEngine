#include "camera.h"

namespace sprint {

Camera::~Camera() {
    FreeID(idx_);
}

int_set<uint32_t, gfx::static_config::kCamerasCapacity>& Camera::Registry() {
    static int_set<uint32_t, gfx::static_config::kCamerasCapacity> idx_registry{};
    return idx_registry;
}

}