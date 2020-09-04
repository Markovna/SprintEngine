#pragma once

#include <cstdint>
#include <string>
#include <array>

#include "Color.h"
#include "VertexLayout.h"

namespace Sprint {

namespace GL {

using CLEAR_FLAG = uint8_t;
constexpr CLEAR_FLAG CLEAR_DEPTH = 0x001;
constexpr CLEAR_FLAG CLEAR_COLOR = 0x002;
constexpr CLEAR_FLAG CLEAR_STENCIL = 0x004;

constexpr static uint16_t INVALID_HANDLE_ID = UINT16_MAX;

struct Handle { uint16_t ID = INVALID_HANDLE_ID; };
inline bool IsValid(Handle handle) { return handle.ID != INVALID_HANDLE_ID; }

struct VertexBufferHandle : public Handle {
    VertexBufferHandle() noexcept : Handle() {}
    VertexBufferHandle(uint16_t id) noexcept : Handle({id}) {}
};
struct IndexBufferHandle : public Handle {
    IndexBufferHandle() noexcept : Handle() {}
    IndexBufferHandle(uint16_t id) noexcept : Handle({id}) {}
};
struct ShaderHandle : public Handle {
    ShaderHandle() noexcept : Handle() {}
    ShaderHandle(uint16_t id) noexcept : Handle({id}) {}
};

struct UniformType {
    enum Type {
        Int,
        Float,
        Vec3,
        Vec4,
        Mat4
    };
};

void Init();
void Shutdown();

IndexBufferHandle CreateIndexBuffer(uint32_t* indices, uint32_t size);
VertexBufferHandle CreateVertexBuffer(float* data, uint32_t size, VertexLayout layout);
ShaderHandle CreateShader(const std::string& source, std::initializer_list<AttributeType::Type> inTypes);

void Bind(IndexBufferHandle handle);
void Bind(VertexBufferHandle handle);

void Destroy(IndexBufferHandle& handle);
void Destroy(VertexBufferHandle& handle);
void Destroy(ShaderHandle& handle);

void SetUniform(ShaderHandle handle, UniformType::Type type, const std::string& name, const void* value);

void Render(ShaderHandle handle);

void SwapBuffers();
void Clear(const Color& color, CLEAR_FLAG options);

}

}
