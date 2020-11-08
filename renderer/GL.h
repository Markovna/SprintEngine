#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Color.h"
#include "VertexLayout.h"
#include "Log.h"

#include "IntAlloc.h"

namespace Sprint {

namespace GL {

constexpr static const uint16_t MAX_INDEX_BUFFERS_COUNT  = 1024;
constexpr static const uint16_t MAX_VERTEX_BUFFERS_COUNT = 1024;
constexpr static const uint16_t MAX_SHADERS_COUNT = 1024;
constexpr static const uint16_t MAX_TEXTURES_COUNT = 1024;

using CLEAR_FLAG = uint8_t;
constexpr static const CLEAR_FLAG CLEAR_DEPTH = 0x001;
constexpr static const CLEAR_FLAG CLEAR_COLOR = 0x002;
constexpr static const CLEAR_FLAG CLEAR_STENCIL = 0x004;

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
    Shader() noexcept : m_AttributesMask(0), m_ID(0) {}
    Shader(const std::string& source, std::initializer_list<AttributeType::Enum> inTypes);

    void Use() const;
    void SetUniform(UniformType::Enum type, const std::string &name, const void* value) const;
    bool TryGetLocation(AttributeType::Enum type, uint32_t& location) const;
    void Destroy();

private:
    uint32_t m_AttributesMask;
    uint32_t m_AttributeLocations[AttributeType::Count];
    uint32_t m_ID;
};

struct IndexBuffer {
    IndexBuffer() noexcept : ID(0), Size(0) {};
    IndexBuffer(uint32_t id, uint32_t size) noexcept : ID(id), Size(size) {}

    uint32_t ID;
    uint32_t Size;
};

struct VertexBuffer {
    VertexBuffer() noexcept : ID(0), Size(0), Layout{} {};
    VertexBuffer(uint32_t id, uint32_t size, VertexLayout layout) noexcept :
            ID(id), Size(size), Layout(layout)
    {}

    uint32_t ID;
    uint32_t Size;
    VertexLayout Layout;
};

struct Texture {
    Texture() noexcept : ID(0) {}
    explicit Texture(uint32_t id) : ID(id) {}

    uint32_t ID;
};

typedef GLFWwindow* ContextWndHandle;

static VertexBufferHandle m_VertexBufferHandle = VertexBufferHandle::Invalid;
static IndexBufferHandle m_IndexBufferHandle = IndexBufferHandle::Invalid;
static uint32_t m_VertexArrayID = 0;
static ContextWndHandle m_CtxWndHandle;

static IndexBuffer m_IndexBuffers[MAX_INDEX_BUFFERS_COUNT];
static VertexBuffer m_VertexBuffers[MAX_VERTEX_BUFFERS_COUNT];
static Shader m_Shaders[MAX_SHADERS_COUNT];
static Texture m_Textures[MAX_TEXTURES_COUNT];

static IntAlloc<MAX_INDEX_BUFFERS_COUNT, IndexBufferHandle::INVALID_ID>  m_IndexBufferIds;
static IntAlloc<MAX_VERTEX_BUFFERS_COUNT, VertexBufferHandle::INVALID_ID> m_VertexBufferIds;
static IntAlloc<MAX_SHADERS_COUNT, ShaderHandle::INVALID_ID> m_ShaderIds;
static IntAlloc<MAX_TEXTURES_COUNT, TextureHandle::INVALID_ID> m_TextureIds;

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

void SetUniform(ShaderHandle handle, UniformType::Enum type, const std::string& name, const void* values);

void Render(ShaderHandle handle);

void SwapBuffers();
void Clear(const Color& color, CLEAR_FLAG options);

}

}
