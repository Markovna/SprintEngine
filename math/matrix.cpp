
#include "matrix.h"

#include <cmath>

#include <cmath>
#include "math_utils.h"

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

Matrix Matrix::Ortho(float x, float width, float y, float height, float minZ, float maxZ) {
    float scale = 1.0f / (maxZ - minZ);
    return Matrix(
        Vec4(2.0f / width, 0.0f, 0.0f, 0.0f),
        Vec4(0.0f, 2.0f / height, 0.0f, 0.0f),
        Vec4(0.0f, 0.0f, scale, 0.0f),
        Vec4(- (2*x + width) / width, - (2*y + height) / height, -(minZ + maxZ) * scale, 1.0f)
    );
}

Matrix Matrix::Ortho(float width, float height, float minZ, float maxZ) {
    float scale = 1.0f / (maxZ - minZ);
    return Matrix(
            Vec4(2.0f / width, 0.0f, 0.0f, 0.0f),
            Vec4(0.0f, 2.0f / height, 0.0f, 0.0f),
            Vec4(0.0f, 0.0f, scale, 0.0f),
            Vec4(0.0f, 0.0f, -(minZ + maxZ) * scale, 1.0f)
        );
}

Matrix Matrix::Perspective(float fov, float ratio, float minZ, float maxZ) {
    assert(math::Approximately(0.0f, 0.0f));
    const float cot = 1.0f / std::tan(fov * 0.5f);
    const float scale = 1.0f / (maxZ - minZ);
    return Matrix(
            Vec4( cot / ratio, 0.0f, 0.0f, 0.0f),
            Vec4(0.0f, cot, 0.0f, 0.0f),
            Vec4(0.0f, 0.0f, maxZ * scale, 1.0f),
            Vec4(0.0f, 0.0f, - minZ * maxZ * scale, 0.0f)
        );
}

Matrix Matrix::LookAt(const Vec3& position, const Vec3& target) {
    Vec3 fwd = Normalized(target - position);
    Vec3 right = Vec3::Up ^ fwd;
    return Matrix(
            right,
            fwd ^ right,
            fwd,
            position
        );
}

Matrix Matrix::LookAt(const Vec3& position, const Vec3& target, const Vec3& up) {
    Vec3 fwd = Normalized(target - position);
    Vec3 right = Normalized(up ^ fwd);
    return Matrix(
            right,
            fwd ^ right,
            fwd,
            position
        );
}

Matrix::Matrix(const Vec3& x, const Vec3& y, const Vec3& z, const Vec3& w) noexcept :
        data_ {
        { x.x, x.y, x.z, 0.0f },
        { y.x, y.y, y.z, 0.0f },
        { z.x, z.y, z.z, 0.0f },
        { w.x, w.y, w.z, 1.0f }
    }
{}

Matrix::Matrix(const Vec4& x, const Vec4& y, const Vec4& z, const Vec4& w) noexcept :
        data_ {
                { x.x, x.y, x.z, x.w },
                { y.x, y.y, y.z, y.w },
                { z.x, z.y, z.z, z.w },
                { w.x, w.y, w.z, w.w }
        }
{}

Matrix::Matrix() noexcept :
        data_ {
        { 1.0f, 0.0f, 0.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f },
        { 0.0f, 0.0f, 1.0f, 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f }
    }
{}

Matrix &Matrix::Inverse() {
    *this = Matrix::GetInverse(*this);
    return *this;
}

Matrix Matrix::GetInverse(const Matrix& mat) {

    Vec3 x = mat.GetRow(0);
    Vec3 y = mat.GetRow(1);
    Vec3 z = mat.GetRow(2);
    Vec3 w = -mat.GetRow(3);

    Matrix res;
    res[0][0] = mat[0][0]; res[0][1] = mat[1][0]; res[0][2] = mat[2][0]; res[0][3] = 0;
    res[1][0] = mat[0][1]; res[1][1] = mat[1][1]; res[1][2] = mat[2][1]; res[1][3] = 0;
    res[2][0] = mat[0][2]; res[2][1] = mat[1][2]; res[2][2] = mat[2][2]; res[2][3] = 0;
    res[3][0] = w | x;     res[3][1] = w | y;     res[3][2] = w | z;     res[3][3] = 1.0f;

    return res;
}

Vec3 Matrix::NormalizeScale() {
    Vec3 scale = {
        GetColumn(0).Length(),
        GetColumn(1).Length(),
        GetColumn(2).Length()
    };

    if (!math::Approximately(scale.x, 0.0f)) {
        float inv = 1 / scale.x;
        data_[0][0] *= inv, data_[0][1] *= inv, data_[0][2] *= inv;
    }
    if (!math::Approximately(scale.y, 0.0f)) {
        float inv = 1 / scale.y;
        data_[1][0] *= inv, data_[1][1] *= inv, data_[1][2] *= inv;
    }
    if (!math::Approximately(scale.z, 0.0f)) {
        float inv = 1 / scale.z;
        data_[2][0] *= inv, data_[2][1] *= inv, data_[2][2] *= inv;
    }

    return scale;
}

Matrix Matrix::TRS(const vec3& origin, const quat& rot, const vec3& scale) {
    Matrix mat;

    const float x2 = rot.x + rot.x, y2 = rot.y + rot.y,     z2 = rot.z + rot.z;
    const float xx = rot.x * x2,    xy = rot.x * y2,        xz = rot.x * z2;
    const float yy = rot.y * y2,    yz = rot.y * z2,        zz = rot.z * z2;
    const float wx = rot.w * x2,    wy = rot.w * y2,        wz = rot.w * z2;

    mat.SetRow(0, { 1.0f - (yy + zz),       (xy + wz) * scale.x,    (xz - wy) * scale.x});
    mat.SetRow(1, { (xy - wz) * scale.y,    1.0f - (xx + zz),       (yz + wx) * scale.y});
    mat.SetRow(2, { (xz + wy) * scale.z,    (yz - wx) * scale.z,    1.0f - (xx + yy) });
    mat.SetRow(3, origin);
    return mat;
}

Matrix GetInverse2(const Matrix& mat) {
    const float det = mat.Determinant();
    if (math::Approximately(det, 0.0f))
    {
        static const float nan = std::numeric_limits<float>::quiet_NaN();
        return Matrix(
            { nan,nan,nan,nan },
            { nan,nan,nan,nan },
            { nan,nan,nan,nan },
            { nan,nan,nan,nan }
        );
    }

    const float inv_det = 1.0f / det;
    const float* row0 = mat[0], *row1 = mat[1], *row2 = mat[2], *row3 = mat[3];

    Vec4 x(row0[0], row0[1], row0[2], row0[3]);
    Vec4 y(row1[0], row1[1], row1[2], row1[3]);
    Vec4 z(row2[0], row2[1], row2[2], row2[3]);
    Vec4 w(row3[0], row3[1], row3[2], row3[3]);

    x[0] = inv_det  * (y[1] * (z[2] * w[3] - w[2] * z[3]) + z[1] * (w[2] * y[3] - y[2] * w[3]) + w[1] * (y[2] * z[3] - z[2] * y[3]));
    y[0] = -inv_det * (y[0] * (z[2] * w[3] - w[2] * z[3]) + z[0] * (w[2] * y[3] - y[2] * w[3]) + w[0] * (y[2] * z[3] - z[2] * y[3]));
    z[0] = inv_det  * (y[0] * (z[1] * w[3] - w[1] * z[3]) + z[0] * (w[1] * y[3] - y[1] * w[3]) + w[0] * (y[1] * z[3] - z[1] * y[3]));
    w[0] = -inv_det * (y[0] * (z[1] * w[2] - w[1] * z[2]) + z[0] * (w[1] * y[2] - y[1] * w[2]) + w[0] * (y[1] * z[2] - z[1] * y[2]));

    x[1] = -inv_det * (x[1] * (z[2] * w[3] - w[2] * z[3]) + z[1] * (w[2] * x[3] - x[2] * w[3]) + w[1] * (x[2] * z[3] - z[2] * x[3]));
    y[1] = inv_det  * (x[0] * (z[2] * w[3] - w[2] * z[3]) + z[0] * (w[2] * x[3] - x[2] * w[3]) + w[0] * (x[2] * z[3] - z[2] * x[3]));
    z[1] = -inv_det * (x[0] * (z[1] * w[3] - w[1] * z[3]) + z[0] * (w[1] * x[3] - x[1] * w[3]) + w[0] * (x[1] * z[3] - z[1] * x[3]));
    w[1] = inv_det  * (x[0] * (z[1] * w[2] - w[1] * z[2]) + z[0] * (w[1] * x[2] - x[1] * w[2]) + w[0] * (x[1] * z[2] - z[1] * x[2]));

    x[2] = inv_det  * (x[1] * (y[2] * w[3] - w[2] * y[3]) + y[1] * (w[2] * x[3] - x[2] * w[3]) + w[1] * (x[2] * y[3] - y[2] * x[3]));
    y[2] = -inv_det * (x[0] * (y[2] * w[3] - w[2] * y[3]) + y[0] * (w[2] * x[3] - x[2] * w[3]) + w[0] * (x[2] * y[3] - y[2] * x[3]));
    z[2] = inv_det  * (x[0] * (y[1] * w[3] - w[1] * y[3]) + y[0] * (w[1] * x[3] - x[1] * w[3]) + w[0] * (x[1] * y[3] - y[1] * x[3]));
    w[2] = -inv_det * (x[0] * (y[1] * w[2] - w[1] * y[2]) + y[0] * (w[1] * x[2] - x[1] * w[2]) + w[0] * (x[1] * y[2] - y[1] * x[2]));

//    x[3] = -inv_det * (x[1] * (y[2] * z[3] - z[2] * y[3]) + y[1] * (z[2] * x[3] - x[2] * z[3]) + z[1] * (x[2] * y[3] - y[2] * x[3]));
//    y[3] = inv_det  * (x[0] * (y[2] * z[3] - z[2] * y[3]) + y[0] * (z[2] * x[3] - x[2] * z[3]) + z[0] * (x[2] * y[3] - y[2] * x[3]));
//    z[3] = -inv_det * (x[0] * (y[1] * z[3] - z[1] * y[3]) + y[0] * (z[1] * x[3] - x[1] * z[3]) + z[0] * (x[1] * y[3] - y[1] * x[3]));
//    w[3] = inv_det  * (x[0] * (y[1] * z[2] - z[1] * y[2]) + y[0] * (z[1] * x[2] - x[1] * z[2]) + z[0] * (x[1] * y[2] - y[1] * x[2]));

    x[3] = 0.0f;
    y[3] = 0.0f;
    z[3] = 0.0f;
    w[3] = 1.0f;

    return Matrix(x, y, z, w);
}

}