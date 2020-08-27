#include "VertexBuffer.h"
#include "Vector.h"

VertexBuffer::VertexBuffer(VertexBuffer &&other) noexcept
        : m_HandleID(other.m_HandleID), m_Layout(std::move(other.m_Layout)), m_Stride(other.m_Stride) {
    other.m_HandleID = 0;
    other.m_Stride = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer &&other) noexcept {
    if (this == &other) return *this;

    if (m_HandleID) {
        glDeleteBuffers(1, &m_HandleID);
    }

    m_HandleID = other.m_HandleID;
    m_Layout   = std::move(other.m_Layout);
    m_Stride   = other.m_Stride;

    other.m_HandleID = 0;
    other.m_Stride = 0;
    return *this;
}

 VertexBuffer VertexBuffer::Create(float* data, uint32_t size, std::initializer_list<VertexDataType> types) {
    assert(types.size());

    std::vector<Item> items;
    items.reserve(types.size());
    size_t offset = 0;
    for (auto type : types) {
        items.push_back({ type, offset, false });
        offset += type.Size;
    }
    return VertexBuffer(move(items), offset, data, size);
}

void VertexBuffer::SetAttribute(const VertexArray& vertexArray, unsigned int slot, size_t attributeIdx) const {
    vertexArray.Bind();
    const Item& item = m_Layout[attributeIdx];
    glVertexAttribPointer(
            slot,
            item.Type.Size,
            item.Type.Type,
            item.Normalized ? GL_TRUE : GL_FALSE,
            m_Stride * sizeof(float),
            (void*)(item.Offset * sizeof(float))
    );
    glEnableVertexAttribArray(slot);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &m_HandleID);
}

VertexBuffer::VertexBuffer(std::vector<Item> items, size_t stride, float* data, uint32_t size) : m_Layout(std::move(items)), m_Stride(stride) {
    glGenBuffers(1, &m_HandleID);
    glBindBuffer(GL_ARRAY_BUFFER, m_HandleID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}