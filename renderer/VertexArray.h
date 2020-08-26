#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "IndexBuffer.h"

class VertexArray {
public:
    VertexArray();
    ~VertexArray();

    VertexArray(const VertexArray& other) = delete;
    VertexArray& operator=(const VertexArray& other) = delete;

    VertexArray(VertexArray&& other) noexcept;
    VertexArray& operator=(VertexArray&& other) noexcept;

    bool operator==(const VertexArray& other) const;

    void Bind() const;
    void SetIndexBuffer(uint32_t* indices, uint32_t count);

    inline size_t IndexCount() const { return m_IndexBuffer.Size(); }


private:
    unsigned int m_HandleID;
    IndexBuffer m_IndexBuffer;
};

