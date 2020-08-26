#include "IndexBuffer.h"

IndexBuffer::IndexBuffer() : m_Size(0) {
    glGenBuffers(1, &m_HandleID);
}

IndexBuffer::~IndexBuffer() {
    if (m_HandleID) {
        glDeleteBuffers(1, &m_HandleID);
    }
}
IndexBuffer::IndexBuffer(IndexBuffer&& other) noexcept :
    m_Size(other.m_Size), m_HandleID(other.m_HandleID) {
    other.m_HandleID = 0;
    other.m_Size = 0;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& other) noexcept {
    if (*this == other) return *this;

    if (m_HandleID) {
        glDeleteBuffers(1, &m_HandleID);
    }

    m_HandleID = other.m_HandleID;
    m_Size     = other.m_Size;

    other.m_HandleID = 0;
    other.m_Size = 0;
    return *this;
}

void IndexBuffer::SetData(uint32_t* indices, size_t size) {
    m_Size = size;
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_HandleID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint32_t), indices, GL_STATIC_DRAW);
}