#pragma once

#include <cstdio>

#include "Vector.h"
#include "Quat.h"


namespace Sprint {

struct Matrix {

    Matrix(const Vec3& x, const Vec3& y, const Vec3& z, const Vec3& w);
    Matrix(const Vec4& x, const Vec4& y, const Vec4& z, const Vec4& w);
    Matrix();

    inline float* operator[](const size_t index) {
        return m_Data[index];
    }

    inline const float* operator[](const size_t index) const {
        return m_Data[index];
    }

    inline Vec3 GetOrigin() const;
    inline void SetOrigin(const Vec3& vec);

    inline Vec3 GetColumn(size_t index) const;
    inline void SetColumn(size_t index, const Vec3& vec);

    inline Matrix operator*(const Matrix& other) const;
    inline void operator*=(const Matrix& other);

    inline Matrix operator+(const Matrix& other) const;
    inline void operator+=(const Matrix& other);

    inline Matrix operator*(float other) const;
    inline void operator*=(float other);

    inline bool operator==(const Matrix& other) const;
    inline bool operator!=(const Matrix& other) const;

    inline bool Equals(const Matrix& other, float tolerance) const;

    inline float Determinant() const;

    inline Vec3 Transform(const Vec3& vec) const;

    static Matrix Translation(const Vec3& trans);
    static Matrix Scale(float scale);
    static Matrix Scale(const Vec3& scale);
    static Matrix Rotation(const Quat& rot);
    static Matrix Ortho(float width, float height, float minZ, float maxZ);
    static Matrix Perspective(float fov, float width, float height, float minZ, float maxZ);
    static Matrix LookAt(const Vec3& position, const Vec3& target);
    static Matrix LookAt(const Vec3& position, const Vec3& target, const Vec3& up);

    const static Matrix Identity;

private:
    float m_Data[4][4];
};

inline Vec3 Matrix::GetOrigin() const {
    return Vec3(m_Data[3][0], m_Data[3][1], m_Data[3][2]);
}

inline void Matrix::SetOrigin(const Vec3& vec) {
    m_Data[3][0] = vec.X;
    m_Data[3][1] = vec.Y;
    m_Data[3][2] = vec.Z;
}

inline Vec3 Matrix::GetColumn(size_t index) const {
    assert(index >= 0 && index < 4);
    return Vec3(m_Data[0][index], m_Data[1][index], m_Data[2][index]);
}

inline void Matrix::SetColumn(size_t index, const Vec3& vec) {
    assert(index >= 0 && index < 4);
    m_Data[0][index] = vec.X;
    m_Data[1][index] = vec.Y;
    m_Data[2][index] = vec.Z;
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
            if (std::abs((*this)[r][c] - other[r][c]) > tolerance) {
                return false;
            }
        }
    }

    return true;
}

inline float Matrix::Determinant() const {
    return	m_Data[0][0] * (
            m_Data[1][1] * (m_Data[2][2] * m_Data[3][3] - m_Data[2][3] * m_Data[3][2]) -
            m_Data[2][1] * (m_Data[1][2] * m_Data[3][3] - m_Data[1][3] * m_Data[3][2]) +
            m_Data[3][1] * (m_Data[1][2] * m_Data[2][3] - m_Data[1][3] * m_Data[2][2]))

              - m_Data[1][0] * (
            m_Data[0][1] * (m_Data[2][2] * m_Data[3][3] - m_Data[2][3] * m_Data[3][2]) -
            m_Data[2][1] * (m_Data[0][2] * m_Data[3][3] - m_Data[0][3] * m_Data[3][2]) +
            m_Data[3][1] * (m_Data[0][2] * m_Data[2][3] - m_Data[0][3] * m_Data[2][2]))

              + m_Data[2][0] * (
            m_Data[0][1] * (m_Data[1][2] * m_Data[3][3] - m_Data[1][3] * m_Data[3][2]) -
            m_Data[1][1] * (m_Data[0][2] * m_Data[3][3] - m_Data[0][3] * m_Data[3][2]) +
            m_Data[3][1] * (m_Data[0][2] * m_Data[1][3] - m_Data[0][3] * m_Data[1][2]))

              - m_Data[3][0] * (
            m_Data[0][1] * (m_Data[1][2] * m_Data[2][3] - m_Data[1][3] * m_Data[2][2]) -
            m_Data[1][1] * (m_Data[0][2] * m_Data[2][3] - m_Data[0][3] * m_Data[2][2]) +
            m_Data[2][1] * (m_Data[0][2] * m_Data[1][3] - m_Data[0][3] * m_Data[1][2]));
}

inline Vec3 Matrix::Transform(const Vec3& vec) const {
    Vec3 res;
    for (size_t r = 0; r < 4; ++r) {
        float sum = 0;
        for (int i = 0; i < 3; ++i) {
            sum += (*this)[r][i] * vec[i];
        }
        res[r] = sum;
    }
    return res;
}

}