#pragma once

#include "vertex_layout.h"

#include <cstdint>
#include <Vector.h>
#include <color.h>
#include <Matrix.h>

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

struct Config {
    void* window_handle;
};

void Init(Config);
void Shutdown();

VertexBufferHandle CreateVertexBuffer(void* data, uint32_t size, VertexLayout layout);
IndexBufferHandle CreateIndexBuffer(void* data, uint32_t size);
ShaderHandle CreateShader(const std::string& source, std::initializer_list<gfx::Attribute::Binding::Enum> in_types);
TextureHandle CreateTexture(uint8_t* data, uint32_t width, uint32_t height, uint32_t channels);

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

void SetBuffer(VertexBufferHandle);
void SetBuffer(IndexBufferHandle);

void SetTransform(const Matrix&);

void SetView(CameraId, const Matrix&);
void SetProjection(CameraId, const Matrix&);
void SetClear(CameraId, ClearFlagMask);
void SetClearColor(CameraId, const Color&);

void Render(CameraId, ShaderHandle);
void Frame();

/*

void Begin(CameraId);
void Draw(ShaderHandle);
void End();
void Render();

 */

/*

 SetView(0, view_camera0);
 SetProjection(0, proj_camera0);
 SetView(1, view_camera1);
 SetProjection(1, proj_camera1);

 Begin(0);

 SetTransform(tr1);
 SetBuffer(vb1);
 SetBuffer(ib1);
 Draw(shader1);

 SetTransform(tr2);
 SetBuffer(vb2);
 SetBuffer(ib2);
 Draw(shader2);

 End(); // end camera0 render

 Begin(1);
 Draw(shader1);
 End(); // end camera1 render

 Render();

*/

};

}