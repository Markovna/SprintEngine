#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <Vector.h>
#include <Matrix.h>

#include "color.h"
#include "VertexLayout.h"
#include "Log.h"

#include "IdAllocator.h"

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
    Texture,
    Camera
};

template<HandleType Type>
struct Handle {
    constexpr static uint16_t INVALID_ID = UINT16_MAX;
    static const Handle Invalid;

    Handle() = default;
    Handle(uint16_t id) : ID(id) {}

    uint16_t ID = INVALID_ID;

    inline bool IsValid() const { return ID != INVALID_ID; }

    bool operator==(Handle other) const {
        return ID == other.ID;
    }

    bool operator!=(Handle other) const {
        return !(*this == other);
    }
};

template <HandleType Type>
const Handle<Type> Handle<Type>::Invalid(INVALID_ID);

using VertexBufferHandle = Handle<HandleType::VertexBuffer>;
using IndexBufferHandle = Handle<HandleType::IndexBuffer>;
using ShaderHandle = Handle<HandleType::Shader>;
using TextureHandle = Handle<HandleType::Texture>;
using CameraHandle = Handle<HandleType::Camera>;

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

using NativeWindowHandle = GLFWwindow*;

struct GraphicContext {
public:
    GraphicContext() {}

    explicit GraphicContext(NativeWindowHandle handle) : handle_(handle) {
    }

    void SwapBuffers() {
        if (handle_)
            glfwSwapBuffers(handle_);
    }

    void MakeCurrent() {
        if (handle_)
            glfwMakeContextCurrent(handle_);
    }
private:
    NativeWindowHandle handle_;
};

using ContextWndHandle = GLFWwindow*;

static VertexBufferHandle vertex_buffer_handle = VertexBufferHandle::Invalid;
static IndexBufferHandle index_buffer_handle = IndexBufferHandle::Invalid;
static uint32_t vertex_array_id = 0;
static GraphicContext default_context;

static IndexBuffer index_buffers[kMaxIndexBuffersCount];
static VertexBuffer vertex_buffers[kMaxVertexBuffersCount];
static Shader shaders[kMaxShadersCount];
static Texture textures[kMaxTexturesCount];

static IdAllocator<kMaxIndexBuffersCount> index_buffer_ids;
static IdAllocator<kMaxVertexBuffersCount> vertex_buffer_ids;
static IdAllocator<kMaxShadersCount> shader_ids;
static IdAllocator<kMaxTexturesCount> texture_ids;

struct Config {
    GraphicContext context;
};

void Init(Config);
void Shutdown();

IndexBufferHandle CreateIndexBuffer(const void* indices, uint32_t size, bool dynamic = false);
VertexBufferHandle CreateVertexBuffer(const void* data, size_t size, VertexLayout layout, bool dynamic = false);
ShaderHandle CreateShader(const std::string& source, std::initializer_list<AttributeType::Enum> in_types);
TextureHandle CreateTexture(const uint8_t* data, uint32_t width, uint32_t height, uint32_t channels);

void Bind(IndexBufferHandle handle);
void Bind(VertexBufferHandle handle);
void Bind(TextureHandle handle, uint32_t idx);

void Destroy(IndexBufferHandle& handle);
void Destroy(VertexBufferHandle& handle);
void Destroy(ShaderHandle& handle);
void Destroy(TextureHandle& handle);

void SetInt(ShaderHandle handle, const std::string& name, int value);
void SetBool(ShaderHandle handle, const std::string& name, bool value);
void SetFloat(ShaderHandle handle, const std::string& name, float value);
void SetFloat3(ShaderHandle handle, const std::string& name, const Vec3& value);
void SetFloat4(ShaderHandle handle, const std::string& name, const Vec4& value);
void SetFloat4(ShaderHandle handle, const std::string &name, const Color &value);
void SetMat(ShaderHandle handle, const std::string& name, const Matrix& value);

void SetUniform(ShaderHandle handle, UniformType::Enum type, const std::string& name, const void* values);

void Render(ShaderHandle handle);

void SwapBuffers();
void Clear(const Color& color, ClearFlag options);

}

}
