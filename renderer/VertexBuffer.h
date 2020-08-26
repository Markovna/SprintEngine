#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Vector.h"
#include "Log.h"
#include "Shader.h"
#include "VertexDataType.h"
#include "IndexBuffer.h"
#include "VertexArray.h"

class VertexBuffer {
private:
    struct Item {
        const VertexDataType Type;
        const size_t Offset;
        bool Normalized;
    };

public:
    static VertexBuffer Create(float* data, uint32_t size, std::initializer_list<VertexDataType> types);

public:
    ~VertexBuffer();

    VertexBuffer(const VertexBuffer& other) = delete;
    VertexBuffer& operator=(const VertexBuffer& other) = delete;

    VertexBuffer(VertexBuffer&& other) noexcept;
    VertexBuffer& operator=(VertexBuffer&& other) noexcept;

    inline void Bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, m_HandleID);
    }

    void SetAttribute(const VertexArray& vertexArray, unsigned int slot, size_t attributeIdx) const;

//        void SetData(float* data, uint32_t size) {
//            glBindBuffer(GL_ARRAY_BUFFER, m_HandleID);
//            glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
//        }


private:
    VertexBuffer(std::vector<Item> items, float* data, uint32_t size);

private:
    unsigned int m_HandleID;
    std::vector<Item> m_Layout;
    size_t m_Stride;
};

