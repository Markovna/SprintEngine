#pragma once

#include "vertex_layout.h"

#include <cstdint>
#include <utility>
#include <Vector.h>
#include <color.h>
#include <Matrix.h>
#include <rect.h>
#include <array>

namespace sprint::gfx {

struct ShaderType {

    enum Enum {
        Vertex = 0,
        Fragment = 1,

        Count
    };

    static bool TryParse(std::string_view str, Enum& type);
};

enum class HandleType {
    VertexBuffer,
    IndexBuffer,
    FrameBuffer,
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

    [[nodiscard]] inline bool IsValid() const { return ID != INVALID_ID; }

    bool operator==(Handle other) const {
        return ID == other.ID;
    }

    bool operator!=(Handle other) const {
        return !(*this == other);
    }
};

template<HandleType Type>
const Handle<Type> Handle<Type>::Invalid(INVALID_ID);

using VertexBufferHandle = Handle<HandleType::VertexBuffer>;
using IndexBufferHandle = Handle<HandleType::IndexBuffer>;
using FrameBufferHandle = Handle<HandleType::FrameBuffer>;
using ShaderHandle = Handle<HandleType::Shader>;
using TextureHandle = Handle<HandleType::Texture>;

using CameraId = uint16_t;
using TexSlotId = uint16_t;

using ClearFlagMask = uint8_t;

struct ClearFlag {
    enum Enum {
        Depth = 0x001,
        Color = 0x002,
        Stencil = 0x004
    };
};

struct TextureFormat {

    enum Enum {
        R8      = 0,
        RGB8,
        RGBA8,

        D24S8,

        Count
    };

    static bool IsDepth(TextureFormat::Enum format) {
        static std::bitset<Count> is_depth = [](){
            std::bitset<Count> is_depth_local;
            is_depth_local[D24S8] = true;
            return is_depth_local;
        }();

        return is_depth[format];
    }

    static uint8_t GetStencilBits(TextureFormat::Enum format) {
        static std::array<uint8_t, Count> stencil_bits = [](){
            std::array<uint8_t, Count> stencil_bits_local{};
            stencil_bits_local.fill(0);
            stencil_bits_local[D24S8] = 8;
            return stencil_bits_local;
        }();
        return stencil_bits[format];
    }

    static uint8_t GetDepthBits(TextureFormat::Enum format) {
        static std::array<uint8_t, Count> depth_bits = [](){
            std::array<uint8_t, Count> depth_bits_local{};
            depth_bits_local.fill(0);
            depth_bits_local[D24S8] = 8;
            return depth_bits_local;
        }();
        return depth_bits[format];
    }
};

struct TextureWrap {
    enum Enum : uint8_t {
        Repeat = 0,
        Mirrored,
        Clamp,
        ClampToBorder
    };

    static TextureWrap All_Repeat() {
        return { Repeat, Repeat, Repeat };
    }

    Enum u = Repeat;
    Enum v = Repeat;
    Enum w = Repeat;
};

struct TextureFilter {
    enum Enum : uint8_t {
        Linear = 0,
        Nearest
    };

    static TextureFilter Default() {
        return { Linear, Linear, Linear };
    }

    Enum min = Linear;
    Enum mag = Linear;
    Enum map = Linear;
};

struct TextureFlags {
    using Type = uint32_t;

    enum Enum {
        None            = 0,
        RenderTarget    = 1u << 0u
    };
};

class MemoryPtr {
public:
    MemoryPtr(std::shared_ptr<void> ptr, uint32_t size) noexcept : ptr_(std::move(ptr)), size_(size) {}

    MemoryPtr() = default;

    MemoryPtr(MemoryPtr&&) = default;
    MemoryPtr(const MemoryPtr&) = default;

    MemoryPtr& operator=(MemoryPtr&& other) noexcept {
        MemoryPtr(std::move(other)).swap(*this);
        return *this;
    }

    MemoryPtr& operator=(const MemoryPtr& other) noexcept {
        MemoryPtr(other).swap(*this);
        return *this;
    }

    void reset() {
        ptr_.reset();
        size_ = 0;
    }

    [[nodiscard]] uint32_t size() const { return size_; }
    [[nodiscard]] void* get() const { return ptr_.get(); }

    explicit operator bool() const noexcept { return get(); }

private:
    void swap(MemoryPtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(size_, other.size_);
    }
private:
    std::shared_ptr<void> ptr_ = nullptr;
    uint32_t size_ = 0;
};

struct Config {
    void* window_handle;
    Vec2Int resolution;
};

struct DrawConfig {
    enum Option {
        NONE = 0,
        DEPTH_TEST = 1
    };

    using Options = uint32_t;
};

void Init(Config);
void Shutdown();

void SetResolution(const Vec2Int&);

VertexBufferHandle CreateVertexBuffer(MemoryPtr ptr, uint32_t size, VertexLayout layout);
IndexBufferHandle CreateIndexBuffer(MemoryPtr ptr, uint32_t size);
FrameBufferHandle CreateFrameBuffer(std::initializer_list<TextureHandle>);
FrameBufferHandle CreateFrameBuffer(uint32_t width, uint32_t height, TextureFormat::Enum, TextureWrap);
ShaderHandle CreateShader(const std::string& source, std::initializer_list<gfx::Attribute::Binding::Enum> in_types);
TextureHandle CreateTexture(uint32_t width, uint32_t height, TextureFormat::Enum, MemoryPtr ref = {});
TextureHandle CreateTexture(uint32_t width, uint32_t height, TextureFormat::Enum, TextureWrap wrap, TextureFilter filter, TextureFlags::Type, MemoryPtr ptr);

void UpdateVertexBuffer(VertexBufferHandle handle, MemoryPtr ptr, uint32_t offset = 0);
void UpdateIndexBuffer(IndexBufferHandle handle, MemoryPtr ptr, uint32_t offset = 0);

void Destroy(VertexBufferHandle&);
void Destroy(IndexBufferHandle&);
void Destroy(FrameBufferHandle&);
void Destroy(ShaderHandle&);
void Destroy(TextureHandle&);

void SetUniform(ShaderHandle, const std::string&, int);
void SetUniform(ShaderHandle, const std::string&, bool);
void SetUniform(ShaderHandle, const std::string&, float);
void SetUniform(ShaderHandle, const std::string&, const Vec3&);
void SetUniform(ShaderHandle, const std::string&, const Vec4&);
void SetUniform(ShaderHandle, const std::string&, const Color&);
void SetUniform(ShaderHandle, const std::string&, const Matrix&);
void SetUniform(ShaderHandle, const std::string&, TextureHandle, TexSlotId);

void SetBuffer(VertexBufferHandle, uint32_t offset = 0, uint32_t num = 0);
void SetBuffer(IndexBufferHandle, uint32_t offset = 0, uint32_t num = 0);

void SetTransform(const Matrix&);

void SetView(CameraId, const Matrix&);
void SetViewRect(CameraId, const Rect& rect);
void SetViewBuffer(CameraId, FrameBufferHandle);
void SetProjection(CameraId, const Matrix&);
void SetClear(CameraId, ClearFlagMask);
void SetClearColor(CameraId, const Color&);
void SetScissor(Rect rect);
void SetOptions(DrawConfig::Options);

MemoryPtr Copy(const void*, uint32_t);
MemoryPtr MakeRef(void*, uint32_t);

void Render(CameraId, ShaderHandle);
void Frame();

}