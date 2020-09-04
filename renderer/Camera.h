#pragma once

#include "Matrix.h"


namespace Sprint {

class Camera {
public:
    explicit Camera(const Matrix& projection) :
        m_Position(Vec3::Zero),
        m_Rotation(Quat::Identity),
        m_ProjectionMatrix(projection)
    {}

    void SetRotation(const Quat& rotation) { m_Rotation = rotation; }
    void SetPosition(const Vec3& position) { m_Position = position; }
    void SetProjectionMatrix(const Matrix& mat) { m_ProjectionMatrix = mat; }

    Matrix GetProjectionMatrix() const { return m_ProjectionMatrix; }
    Matrix GetViewMatrix() const { return Matrix::Translation(m_Position) * Matrix::Rotation(m_Rotation); }

private:
    Matrix m_ProjectionMatrix;

    Vec3 m_Position;
    Quat m_Rotation;
};

}

