#include "VertexDataType.h"

const VertexDataType VertexDataType::Float(GL_FLOAT, 1);
const VertexDataType VertexDataType::Vec2(GL_FLOAT, 2);
const VertexDataType VertexDataType::Vec3(GL_FLOAT, 3);
const VertexDataType VertexDataType::Vec4(GL_FLOAT, 4);

VertexDataType::VertexDataType(GLenum type, GLint size) noexcept : Type(type), Size(size) {}