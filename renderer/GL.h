#pragma once

#include <string>

#include "Color.h"
#include "VertexLayout.h"

namespace Sprint {

namespace GL {

using CLEAR_FLAG = uint8_t;
constexpr CLEAR_FLAG CLEAR_DEPTH = 0x001;
constexpr CLEAR_FLAG CLEAR_COLOR = 0x002;
constexpr CLEAR_FLAG CLEAR_STENCIL = 0x004;

enum class HandleType {
    VertexBuffer,
    IndexBuffer,
    Shader,
    Texture
};

template<HandleType Type>
struct Handle {
    constexpr static uint16_t INVALID_ID = UINT16_MAX;
    static const Handle Invalid;

    Handle() = default;
    Handle(uint16_t id) : ID(id) {}

    uint16_t ID = INVALID_ID;

    inline bool IsValid() const { return ID != INVALID_ID; }
};

template <HandleType Type>
const Handle<Type> Handle<Type>::Invalid(INVALID_ID);

typedef Handle<HandleType::VertexBuffer> VertexBufferHandle;
typedef Handle<HandleType::IndexBuffer> IndexBufferHandle;
typedef Handle<HandleType::Shader> ShaderHandle;
typedef Handle<HandleType::Texture> TextureHandle;

struct UniformType {
    enum Enum {
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
ShaderHandle CreateShader(const std::string& source, std::initializer_list<AttributeType::Enum> inTypes);
TextureHandle CreateTexture(const uint8_t* data, uint32_t width, uint32_t height, uint32_t channels);

void Bind(IndexBufferHandle handle);
void Bind(VertexBufferHandle handle);
void Bind(TextureHandle handle, uint32_t idx);

void Destroy(IndexBufferHandle& handle);
void Destroy(VertexBufferHandle& handle);
void Destroy(ShaderHandle& handle);
void Destroy(TextureHandle& handle);

void SetUniform(ShaderHandle handle, UniformType::Enum type, const std::string& name, const void* value);

void Render(ShaderHandle handle);

void SwapBuffers();
void Clear(const Color& color, CLEAR_FLAG options);

}

}
