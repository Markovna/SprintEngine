#pragma once

#include "vertex_layout.h"

#include <cstdint>
#include <utility>
#include <Vector.h>
#include <color.h>
#include <Matrix.h>
#include <rect.h>

namespace sprint {

namespace gfx {

struct ShaderType {
    enum Enum { Vertex = 0, Fragment = 1 };

    constexpr static const size_t Count = 2;

    static bool TryParse(std::string_view str, Enum& type);
};

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

template<HandleType Type>
const Handle<Type> Handle<Type>::Invalid(INVALID_ID);

using VertexBufferHandle = Handle<HandleType::VertexBuffer>;
using IndexBufferHandle = Handle<HandleType::IndexBuffer>;
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

    uint32_t size() const { return size_; }
    void* get() const { return ptr_.get(); }

    explicit operator bool() const noexcept { return get() != 0; }

private:
    void swap(MemoryPtr& other) noexcept {
        std::swap(ptr_, other.ptr_);
        std::swap(size_, other.size_);
    }
private:
    std::shared_ptr<void> ptr_;
    uint32_t size_ = 0;
};

struct Config {
    void* window_handle;
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

VertexBufferHandle CreateVertexBuffer(MemoryPtr ptr, uint32_t size, VertexLayout layout);
IndexBufferHandle CreateIndexBuffer(MemoryPtr ptr, uint32_t size);
ShaderHandle CreateShader(const std::string& source, std::initializer_list<gfx::Attribute::Binding::Enum> in_types);
TextureHandle CreateTexture(MemoryPtr ref, uint32_t width, uint32_t height, uint32_t channels);

void UpdateVertexBuffer(VertexBufferHandle handle, MemoryPtr ptr, uint32_t offset = 0);
void UpdateIndexBuffer(IndexBufferHandle handle, MemoryPtr ptr, uint32_t offset = 0);

void Destroy(VertexBufferHandle&);
void Destroy(IndexBufferHandle&);
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
void SetProjection(CameraId, const Matrix&);
void SetClear(CameraId, ClearFlagMask);
void SetClearColor(CameraId, const Color&);
void SetScissor(Rect rect);
void SetOptions(DrawConfig::Options);

MemoryPtr Copy(const void*, uint32_t);
MemoryPtr MakeRef(void*, uint32_t);

void Render(CameraId, ShaderHandle);
void Frame();

};

}