#pragma once

#include <cstdint>
#include <cstdio>

namespace sprint {

namespace gfx {

namespace static_config {

constexpr static const uint16_t kAttributesCapacity = 16;
constexpr static const uint16_t kCamerasCapacity = 16;
constexpr static const uint16_t kIndexBuffersCapacity = 1024;
constexpr static const uint16_t kVertexBuffersCapacity = 1024;
constexpr static const uint16_t kShadersCapacity = 1024;
constexpr static const uint16_t kTexturesCapacity = 1024;
constexpr static const size_t kMaxDrawCallsCount = 2048;
constexpr static const uint16_t kMaxCommandCountPerDrawCall = 2048;
constexpr static const uint16_t kMaxFrameCommandsCount = 2048;

};

}

}

