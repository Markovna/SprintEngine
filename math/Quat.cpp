#include "Quat.h"


namespace Sprint {

Quat::Quat() {}

Quat::Quat(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w) {}

Quat::Quat(const Vec3& Axis, float AngleRad) {
    const float halfAngle = 0.5f * AngleRad;
    const float s = std::sin(halfAngle);
    X = s * Axis.X; Y = s * Axis.Y; Z = s * Axis.Z; W = std::cos(halfAngle);
}

const Quat Quat::Identity(0.0f,0.0f,0.0f,1.0f);

}