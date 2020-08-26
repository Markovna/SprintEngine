#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class IndexBuffer {
public:
    IndexBuffer();
    ~IndexBuffer();

    IndexBuffer(const IndexBuffer& other) = delete;
    IndexBuffer& operator=(const IndexBuffer& other) = delete;

    IndexBuffer(IndexBuffer&& other) noexcept;
    IndexBuffer& operator=(IndexBuffer&& other) noexcept;

    void SetData(uint32_t* indices, size_t size);

    inline size_t Size() const { return m_Size; }

    inline bool operator==(const IndexBuffer& other) const { return m_HandleID == other.m_HandleID; }

private:
    unsigned int m_HandleID;
    size_t m_Size;
};