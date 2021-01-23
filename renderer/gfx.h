#pragma once

#include "vertex_layout.h"

#include "Vector.h"
#include "color.h"
#include "Matrix.h"
#include "rect.h"
#include <array>

namespace sprint::gfx {

enum HandleType {
    VertexBuffer = 0,
    IndexBuffer,
    FrameBuffer,
    Shader,
    Uniform,
    Texture
};

template<class T>
struct handle_traits;

template<>
struct handle_traits<uint32_t> {
    static constexpr uint32_t index_mask       = 0x000FFFFFu;
    static constexpr uint8_t  generation_offs  = 20u;
    static constexpr uint32_t generation_mask  = 0xFFFu << generation_offs;
};


template<size_t>
struct handle {
public:
    using type = uint32_t;
    type id = null_t;
private:
    using traits = handle_traits<type>;
    constexpr static const type null_t = traits::index_mask;

public:
    static const handle null;

    explicit constexpr operator bool() const noexcept {
        return id != null_t;
    }

    bool operator==(handle other) const noexcept {
        return id == other.id;
    }

    bool operator!=(handle other) const noexcept {
        return !(*this == other);
    }

    [[nodiscard]] inline type index() const { return id & traits::index_mask; }
};

template <size_t Id>
const handle<Id> handle<Id>::null = { null_t };

template<class T, size_t Capacity>
class handle_pool {
private:
    using type = typename T::type;
    using traits = handle_traits<type>;

    constexpr static const type null = traits::index_mask;
public:
    T get() {
        type idx{};
        if (free_ == null) {
            idx = size_;
            store_[size_++] = idx;
        } else {
            auto free = store_[free_];
            store_[free_] = idx = free_ | (free & traits::generation_mask);
            free_ = free & traits::index_mask;
        }

        return T{idx};
    }

    void erase(T value) {
        auto index = value.index();
        auto generation = ((value.id >> traits::generation_offs) + 1) << traits::generation_offs;
        store_[index] = free_ | generation;
        free_ = index;
    }

    bool valid(T value) const {
        auto index = value.index();
        return index < Capacity && index < size_ && store_[index] == value.id;
    }

private:
    type store_[Capacity];
    type size_ = 0;
    type free_ = null;
};

struct ShaderType {

    enum Enum {
        Vertex = 0,
        Fragment = 1,

        Count
    };

    static bool TryParse(std::string_view str, Enum& type);
};

using vertexbuf_handle = handle<HandleType::VertexBuffer>;
using indexbuf_handle = handle<HandleType::IndexBuffer>;
using framebuf_handle = handle<HandleType::FrameBuffer>;
using shader_handle = handle<HandleType::Shader>;
using uniform_handle = handle<HandleType::Uniform>;
using texture_handle = handle<HandleType::Texture>;

using CameraId = uint16_t;
using TexSlotId = uint16_t;

struct ClearFlag {
    enum Enum {
        Depth = 0x001,
        Color = 0x002,
        Stencil = 0x004
    };

    using Type = uint8_t;
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

    Enum u = Repeat;
    Enum v = Repeat;
    Enum w = Repeat;
};

struct TextureFilter {
    enum Enum : uint8_t {
        Linear = 0,
        Nearest
    };

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
    void* window_handle{};
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

vertexbuf_handle CreateVertexBuffer(MemoryPtr ptr, uint32_t size, VertexLayout layout);
indexbuf_handle CreateIndexBuffer(MemoryPtr ptr, uint32_t size);
framebuf_handle CreateFrameBuffer(std::initializer_list<texture_handle>);
framebuf_handle CreateFrameBuffer(uint32_t width, uint32_t height, TextureFormat::Enum, TextureWrap);
uniform_handle CreateUniform(const char* c_str);
shader_handle CreateShader(const std::string& source, std::initializer_list<gfx::Attribute::Binding::Enum> in_types);
texture_handle CreateTexture(uint32_t width, uint32_t height, TextureFormat::Enum, MemoryPtr ref = {});
texture_handle CreateTexture(uint32_t width, uint32_t height, TextureFormat::Enum, TextureWrap wrap, TextureFilter filter, TextureFlags::Type, MemoryPtr ptr);

void UpdateVertexBuffer(vertexbuf_handle handle, MemoryPtr ptr, uint32_t offset = 0);
void UpdateIndexBuffer(indexbuf_handle handle, MemoryPtr ptr, uint32_t offset = 0);

void Destroy(vertexbuf_handle&);
void Destroy(indexbuf_handle&);
void Destroy(framebuf_handle&);
void Destroy(shader_handle&);
void Destroy(uniform_handle&);
void Destroy(texture_handle&);

bool IsValid(vertexbuf_handle);
bool IsValid(indexbuf_handle);
bool IsValid(framebuf_handle);
bool IsValid(shader_handle);
bool IsValid(uniform_handle);
bool IsValid(texture_handle);

void SetUniform(uniform_handle, int);
void SetUniform(uniform_handle, bool);
void SetUniform(uniform_handle, float);
void SetUniform(uniform_handle, const Vec3&);
void SetUniform(uniform_handle, const Vec4&);
void SetUniform(uniform_handle, const Color&);
void SetUniform(uniform_handle, const Matrix&);
void SetUniform(uniform_handle, texture_handle, TexSlotId);

void SetBuffer(vertexbuf_handle, uint32_t offset = 0, uint32_t num = 0);
void SetBuffer(indexbuf_handle, uint32_t offset = 0, uint32_t num = 0);

void SetTransform(const Matrix&);

void SetView(CameraId, const Matrix&);
void SetViewRect(CameraId, const Rect& rect);
void SetViewBuffer(CameraId, framebuf_handle);
void SetProjection(CameraId, const Matrix&);
void SetClear(CameraId, ClearFlag::Type);
void SetClearColor(CameraId, const Color&);
void SetScissor(Rect rect);
void SetOptions(DrawConfig::Options);

MemoryPtr Copy(const void*, uint32_t);
MemoryPtr MakeRef(void*, uint32_t);

void Render(CameraId, shader_handle);
void Frame();

}