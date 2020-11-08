#include "Matrix.h"


namespace sprint {

const Matrix Matrix::Identity(Vec3::Forward, Vec3::Right, Vec3::Up, Vec3::Zero);

Matrix Matrix::Translation(const Vec3& trans) {
    return Matrix(Vec3::Forward, Vec3::Right, Vec3::Up, trans);
}

Matrix Matrix::Scale(float scale) {
    return Matrix(
            Vec3(scale, 0.0f, 0.0f),
            Vec3(0.0f, scale, 0.0f),
            Vec3(0.0f, 0.0f, scale),
            Vec3(0.0f, 0.0f, 0.0f)
        );
}

Matrix Matrix::Scale(const Vec3& scale) {
    return Matrix(
            Vec3(scale.x, 0.0f, 0.0f),
            Vec3(0.0f, scale.y, 0.0f),
            Vec3(0.0f, 0.0f, scale.z),
            Vec3(0.0f, 0.0f, 0.0f)
        );
}

Matrix Matrix::Rotation(const Quat& rot) {
    const float x2 = rot.x + rot.x;  const float y2 = rot.y + rot.y;  const float z2 = rot.z + rot.z;
    const float xx = rot.x * x2;     const float xy = rot.x * y2;   const float xz = rot.x * z2;
    const float yy = rot.y * y2;     const float yz = rot.y * z2;   const float zz = rot.z * z2;
    const float wx = rot.w * x2;     const float wy = rot.w * y2;   const float wz = rot.w * z2;

    return Matrix(
            Vec3(1.0f - (yy + zz), xy + wz,          xz - wy),
            Vec3(xy - wz,          1.0f - (xx + zz), yz + wx),
            Vec3(xz + wy,          yz - wx,          1.0f - (xx + yy)),
            Vec3(0.0f,             0.0f,             0.0f)
        );
}

Matrix Matrix::Ortho(float width, float height, float minZ, float maxZ) {
    float scale = 1.0f / (maxZ - minZ);
    return Matrix(
            Vec4(1.0f / width, 0.0f, 0.0f, 0.0f),
            Vec4(0.0f, 1.0f / height, 0.0f, 0.0f),
            Vec4(0.0f, 0.0f, scale, 0.0f),
            Vec4(0.0f, 0.0f, -(minZ + maxZ) * scale, 1.0f)
        );
}

Matrix Matrix::Perspective(float fov, float width, float height, float minZ, float maxZ) {
    const float tan = 1.0f / std::tan(fov * 0.5f);
    const float scale = 1.0f / (maxZ - minZ);
    return Matrix(
            Vec4(tan, 0.0f, 0.0f, 0.0f),
            Vec4(0.0f, width * tan / height, 0.0f, 0.0f),
            Vec4(0.0f, 0.0f, maxZ * scale, 1.0f),
            Vec4(0.0f, 0.0f, - minZ * maxZ * scale, 0.0f)
        );
}

Matrix Matrix::LookAt(const Vec3& position, const Vec3& target) {
    Vec3 fwd = Normalized(position - target);
    Vec3 right = Vec3::Up ^ fwd;
    return Matrix(
            right,
            fwd ^ right,
            fwd,
            position
        );
}

Matrix Matrix::LookAt(const Vec3& position, const Vec3& target, const Vec3& up) {
    Vec3 fwd = Normalized(position - target);
    Vec3 right = Normalized(up ^ fwd);
    return Matrix(
            right,
            fwd ^ right,
            fwd,
            position
        );
}

Matrix::Matrix(const Vec3& x, const Vec3& y, const Vec3& z, const Vec3& w) :
        data_ {
        { x.x, x.y, x.z, 0.0f },
        { y.x, y.y, y.z, 0.0f },
        { z.x, z.y, z.z, 0.0f },
        { w.x, w.y, w.z, 1.0f }
    }
{}

Matrix::Matrix(const Vec4& x, const Vec4& y, const Vec4& z, const Vec4& w) :
        data_ {
                { x.x, x.y, x.z, x.w },
                { y.x, y.y, y.z, y.w },
                { z.x, z.y, z.z, z.w },
                { w.x, w.y, w.z, w.w }
        }
{}

Matrix::Matrix() :
        data_ {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f }
    }
{}

}