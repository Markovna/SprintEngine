#include "VertexArray.h"

VertexArray::VertexArray() : m_IndexBuffer() {
    glGenVertexArrays(1, &m_HandleID);
}

VertexArray::~VertexArray() {
    if (m_HandleID) {
        glDeleteVertexArrays(1, &m_HandleID);
    }
}

void VertexArray::Bind() const {
    glBindVertexArray(m_HandleID);
}

void VertexArray::SetIndexBuffer(uint32_t* indices, uint32_t count) {
    glBindVertexArray(m_HandleID);
    m_IndexBuffer.SetData(indices, count);
}

VertexArray::VertexArray(VertexArray &&other) noexcept :
    m_HandleID(other.m_HandleID), m_IndexBuffer(std::move(other.m_IndexBuffer)) {
    other.m_HandleID = 0;
}

VertexArray& VertexArray::operator=(VertexArray&& other) noexcept {
    if (*this == other) return *this;

    if (m_HandleID) {
        glDeleteVertexArrays(1, &m_HandleID);
    }

    m_HandleID = other.m_HandleID;
    m_IndexBuffer = std::move(other.m_IndexBuffer);

    other.m_HandleID = 0;
    return *this;
}

bool VertexArray::operator==(const VertexArray &other) const {
    return m_HandleID == other.m_HandleID;
}
