#include "Matrix.h"


namespace Sprint {

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
            Vec3(scale.X, 0.0f, 0.0f),
            Vec3(0.0f, scale.Y, 0.0f),
            Vec3(0.0f, 0.0f, scale.Z),
            Vec3(0.0f, 0.0f, 0.0f)
        );
}

Matrix Matrix::Rotation(const Quat& rot) {
    const float x2 = rot.X + rot.X;  const float y2 = rot.Y + rot.Y;  const float z2 = rot.Z + rot.Z;
    const float xx = rot.X * x2;     const float xy = rot.X * y2;   const float xz = rot.X * z2;
    const float yy = rot.Y * y2;     const float yz = rot.Y * z2;   const float zz = rot.Z * z2;
    const float wx = rot.W * x2;     const float wy = rot.W * y2;   const float wz = rot.W * z2;

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
    m_Data {
        { x.X, x.Y, x.Z, 0.0f },
        { y.X, y.Y, y.Z, 0.0f },
        { z.X, z.Y, z.Z, 0.0f },
        { w.X, w.Y, w.Z, 1.0f }
    }
{}

Matrix::Matrix(const Vec4& x, const Vec4& y, const Vec4& z, const Vec4& w) :
        m_Data {
                { x.X, x.Y, x.Z, x.W },
                { y.X, y.Y, y.Z, y.W },
                { z.X, z.Y, z.Z, z.W },
                { w.X, w.Y, w.Z, w.W }
        }
{}

Matrix::Matrix() :
    m_Data {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f }
    }
{}

}