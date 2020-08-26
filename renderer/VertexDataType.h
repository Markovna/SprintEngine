#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct VertexDataType {
public:
    const static VertexDataType Float;
    const static VertexDataType Vec2;
    const static VertexDataType Vec3;
    const static VertexDataType Vec4;
public:
    const GLenum Type;
    const GLint Size;
private:
    VertexDataType(GLenum type, GLint size) noexcept;
};
