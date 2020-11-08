#include "Quat.h"


namespace sprint {

Quat::Quat() {}

Quat::Quat(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

Quat::Quat(const Vec3& axis, float angle_rad) {
    const float halfAngle = 0.5f * angle_rad;
    const float s = std::sin(halfAngle);
    x = s * axis.x; y = s * axis.y; z = s * axis.z; w = std::cos(halfAngle);
}

const Quat Quat::Identity(0.0f,0.0f,0.0f,1.0f);

}