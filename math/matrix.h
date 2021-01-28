#pragma once

#include "vector.h"
#include "quat.h"

namespace sprint {

struct Matrix;

using mat4 = Matrix;

struct Matrix {

    Matrix(const Vec3& x, const Vec3& y, const Vec3& z, const Vec3& w) noexcept;
    Matrix(const Vec4& x, const Vec4& y, const Vec4& z, const Vec4& w) noexcept;
    Matrix() noexcept;

    inline float* operator[](const size_t index) {
        return data_[index];
    }

    inline const float* operator[](const size_t index) const {
        return data_[index];
    }

    [[nodiscard]] inline Vec3 GetOrigin() const;
    inline void SetOrigin(const Vec3& vec);

    [[nodiscard]] inline Vec3 GetColumn(size_t index) const;
    inline void SetColumn(size_t index, const Vec3& vec);

    [[nodiscard]] inline Vec3 GetRow(size_t index) const;
    inline void SetRow(size_t index, const Vec3& vec);

    Vec3 NormalizeScale();

    inline Matrix operator*(const Matrix& other) const;
    inline void operator*=(const Matrix& other);

    inline Matrix operator+(const Matrix& other) const;
    inline void operator+=(const Matrix& other);

    inline Matrix operator*(float other) const;
    inline void operator*=(float other);

    inline bool operator==(const Matrix& other) const;
    inline bool operator!=(const Matrix& other) const;

    [[nodiscard]] inline bool Equals(const Matrix& other, float tolerance) const;

    [[nodiscard]] inline float Determinant() const;
    Matrix& Inverse();

    [[nodiscard]] inline Vec3 Transform(const Vec3& vec) const;
    [[nodiscard]] inline Vec4 Transform(const Vec4& vec) const;

    static Matrix GetInverse(const Matrix&);
    static Matrix Translation(const Vec3& trans);
    static Matrix Scale(float scale);
    static Matrix Scale(const Vec3& scale);
    static Matrix Rotation(const Quat& rot);
    static Matrix TRS(const vec3&, const quat&, const vec3&);
    static Matrix Ortho(float x, float width, float y, float height, float minZ, float maxZ);
    static Matrix Ortho(float width, float height, float minZ, float maxZ);
    static Matrix Perspective(float fov, float width, float height, float minZ, float maxZ);
    static Matrix LookAt(const Vec3& position, const Vec3& target);
    static Matrix LookAt(const Vec3& position, const Vec3& target, const Vec3& up);

    const static Matrix Identity;

private:
    float data_[4][4];
};

inline Vec3 Matrix::GetOrigin() const {
    return Vec3(data_[3][0], data_[3][1], data_[3][2]);
}

inline void Matrix::SetOrigin(const Vec3& vec) {
    data_[3][0] = vec.x; data_[3][1] = vec.y; data_[3][2] = vec.z;
}

inline Vec3 Matrix::GetRow(size_t index) const {
    assert(index >= 0 && index < 4);
    return Vec3(data_[index][0], data_[index][1], data_[index][2]);
}

void Matrix::SetRow(size_t index, const Vec3& vec) {
    assert(index >= 0 && index < 4);
    data_[index][0] = vec[0], data_[index][1] = vec[1], data_[index][2] = vec[2];
}

inline Vec3 Matrix::GetColumn(size_t index) const {
    assert(index >= 0 && index < 4);
    return Vec3(data_[0][index], data_[1][index], data_[2][index]);
}

inline void Matrix::SetColumn(size_t index, const Vec3& vec) {
    assert(index >= 0 && index < 4);
    data_[0][index] = vec.x, data_[1][index] = vec.y, data_[2][index] = vec.z;
}

inline Matrix Matrix::operator*(const Matrix& other) const {
    Matrix mat;
    for (size_t r = 0; r < 4; ++r) {
        float sum;
        for (size_t c = 0; c < 4; ++c) {
            sum = 0;
            for (int i = 0; i < 4; ++i) {
                sum += (*this)[r][i] * other[i][c];
            }
            mat[r][c] = sum;
        }
    }
    return mat;
}

inline void Matrix::operator*=(const Matrix& other) {
    *this = *this * other;
}

inline Matrix Matrix::operator+(const Matrix& other) const {
    Matrix mat;
    for (size_t r = 0; r < 4; ++r) {
        for (size_t c = 0; c < 4; ++c) {
            mat[r][c] = mat[r][c] + other[r][c];
        }
    }

    return mat;
}

inline void Matrix::operator+=(const Matrix& other) {
    *this = *this + other;
}

inline Matrix Matrix::operator*(float other) const {
    Matrix mat;
    for (size_t r = 0; r < 4; ++r) {
        for (size_t c = 0; c < 4; ++c) {
            mat[r][c] = mat[r][c] * other;
        }
    }

    return mat;
}

inline void Matrix::operator*=(float other) {
    *this = *this * other;
}

inline bool Matrix::operator==(const Matrix& other) const {
    for (size_t r = 0; r < 4; ++r) {
        for (size_t c = 0; c < 4; ++c) {
            if ((*this)[r][c] != other[r][c])
                return false;
        }
    }

    return true;
}

inline bool Matrix::operator!=(const Matrix& other) const {
    return !(*this == other);
}

inline bool Matrix::Equals(const Matrix& other, float tolerance) const {
    for (size_t r = 0; r < 4; ++r) {
        for (size_t c = 0; c < 4; ++c) {
            if (std::fabs((*this)[r][c] - other[r][c]) > tolerance) {
                return false;
            }
        }
    }

    return true;
}

inline float Matrix::Determinant() const {
    return data_[0][0] * (
            data_[1][1] * (data_[2][2] * data_[3][3] - data_[2][3] * data_[3][2]) -
            data_[2][1] * (data_[1][2] * data_[3][3] - data_[1][3] * data_[3][2]) +
            data_[3][1] * (data_[1][2] * data_[2][3] - data_[1][3] * data_[2][2]))

           - data_[1][0] * (
            data_[0][1] * (data_[2][2] * data_[3][3] - data_[2][3] * data_[3][2]) -
            data_[2][1] * (data_[0][2] * data_[3][3] - data_[0][3] * data_[3][2]) +
            data_[3][1] * (data_[0][2] * data_[2][3] - data_[0][3] * data_[2][2]))

           + data_[2][0] * (
            data_[0][1] * (data_[1][2] * data_[3][3] - data_[1][3] * data_[3][2]) -
            data_[1][1] * (data_[0][2] * data_[3][3] - data_[0][3] * data_[3][2]) +
            data_[3][1] * (data_[0][2] * data_[1][3] - data_[0][3] * data_[1][2]))

           - data_[3][0] * (
            data_[0][1] * (data_[1][2] * data_[2][3] - data_[1][3] * data_[2][2]) -
            data_[1][1] * (data_[0][2] * data_[2][3] - data_[0][3] * data_[2][2]) +
            data_[2][1] * (data_[0][2] * data_[1][3] - data_[0][3] * data_[1][2]));
}

inline Vec3 Matrix::Transform(const Vec3& vec) const {
    Vec3 res;
    for (size_t r = 0; r < 3; ++r) {
        float sum = 0;
        for (int i = 0; i < 3; ++i) {
            sum += (*this)[r][i] * vec[i];
        }
        res[r] = sum;
    }
    return res;
}

inline Vec4 Matrix::Transform(const Vec4& vec) const {
    Vec4 res;
    for (size_t r = 0; r < 4; ++r) {
        float sum = 0;
        for (int i = 0; i < 4; ++i) {
            sum += (*this)[r][i] * vec[i];
        }
        res[r] = sum;
    }
    return res;
}

}