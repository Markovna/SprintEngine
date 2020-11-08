#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Color.h"
#include "VertexLayout.h"
#include "Log.h"

#include "IntAlloc.h"

namespace sprint {

namespace gl {

constexpr static const uint16_t kMaxIndexBuffersCount  = 1024;
constexpr static const uint16_t kMaxVertexBuffersCount = 1024;
constexpr static const uint16_t kMaxShadersCount = 1024;
constexpr static const uint16_t kMaxTexturesCount = 1024;

using ClearFlag = uint8_t;
constexpr static const ClearFlag kClearDepth = 0x001;
constexpr static const ClearFlag kClearColor = 0x002;
constexpr static const ClearFlag kClearStencil = 0x004;

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

    static std::string ToString(Enum type);
};

struct ShaderType {
    enum Enum {
        Vertex   = 0,
        Fragment = 1
    };

    constexpr static const size_t Count = 2;

    static bool TryParse(std::string_view str, Enum& type);
};

class Shader {
public:
    Shader() noexcept : attributes_mask_(0), id_(0) {}
    Shader(const std::string& source, std::initializer_list<AttributeType::Enum> inTypes);

    void Use() const;
    void SetUniform(UniformType::Enum type, const std::string &name, const void* value) const;
    bool TryGetLocation(AttributeType::Enum type, uint32_t& location) const;
    void Destroy();

private:
    uint32_t attributes_mask_;
    uint32_t attribute_locations_[AttributeType::Count];
    uint32_t id_;
};

struct IndexBuffer {
    IndexBuffer() noexcept : id(0), size(0) {};
    IndexBuffer(uint32_t id, uint32_t size) noexcept : id(id), size(size) {}

    uint32_t id;
    uint32_t size;
};

struct VertexBuffer {
    VertexBuffer() noexcept : id(0), size(0), layout{} {};
    VertexBuffer(uint32_t id, uint32_t size, VertexLayout layout) noexcept :
        id(id), size(size), layout(layout)
    {}

    uint32_t id;
    uint32_t size;
    VertexLayout layout;
};

struct Texture {
    Texture() noexcept : id(0) {}
    explicit Texture(uint32_t id) : id(id) {}

    uint32_t id;
};

typedef GLFWwindow* ContextWndHandle;

static VertexBufferHandle vertex_buffer_handle = VertexBufferHandle::Invalid;
static IndexBufferHandle index_buffer_handle = IndexBufferHandle::Invalid;
static uint32_t vertex_array_id = 0;
static ContextWndHandle ctx_window_handle;

static IndexBuffer index_buffers[kMaxIndexBuffersCount];
static VertexBuffer vertex_buffers[kMaxVertexBuffersCount];
static Shader shaders[kMaxShadersCount];
static Texture textures[kMaxTexturesCount];

static IntAlloc<kMaxIndexBuffersCount, IndexBufferHandle::INVALID_ID>  index_buffer_ids;
static IntAlloc<kMaxVertexBuffersCount, VertexBufferHandle::INVALID_ID> vertex_buffer_ids;
static IntAlloc<kMaxShadersCount, ShaderHandle::INVALID_ID> shader_ids;
static IntAlloc<kMaxTexturesCount, TextureHandle::INVALID_ID> texture_ids;

void Init();
void Shutdown();

IndexBufferHandle CreateIndexBuffer(uint32_t* indices, uint32_t size);
VertexBufferHandle CreateVertexBuffer(float* data, uint32_t size, VertexLayout layout);
ShaderHandle CreateShader(const std::string& source, std::initializer_list<AttributeType::Enum> in_types);
TextureHandle CreateTexture(const uint8_t* data, uint32_t width, uint32_t height, uint32_t channels);

void Bind(IndexBufferHandle handle);
void Bind(VertexBufferHandle handle);
void Bind(TextureHandle handle, uint32_t idx);

void Destroy(IndexBufferHandle& handle);
void Destroy(VertexBufferHandle& handle);
void Destroy(ShaderHandle& handle);
void Destroy(TextureHandle& handle);

void SetUniform(ShaderHandle handle, UniformType::Enum type, const std::string& name, const void* values);

void Render(ShaderHandle handle);

void SwapBuffers();
void Clear(const Color& color, ClearFlag options);

}

}
