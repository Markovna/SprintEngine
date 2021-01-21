#include "gfx.h"

#include <utility>
#include "gfx_details.h"
#include "Log.h"
#include "../debug/profiler.h"

namespace sprint {

namespace gfx {

void Init(Config config) {
    log::core::Info("gfx::Init");
    details::g_config = config;
    details::g_renderer.Init(details::g_config);
}

void Shutdown() {
    details::g_renderer.Shutdown();
    log::core::Info("gfx::Shutdown");
}

bool ShaderType::TryParse(std::string_view str, ShaderType::Enum &type) {
    if (str == "vertex") {
        type = Vertex;
        return true;
    }
    if (str == "fragment") {
        type = Fragment;
        return true;
    }
    return false;
}

VertexBufferHandle CreateVertexBuffer(MemoryPtr ptr, uint32_t size, VertexLayout layout) {
    log::core::Info("gfx::CreateVertexBuffer");
    return details::g_renderer.CreateVertexBuffer(std::move(ptr), size, layout);
}

IndexBufferHandle CreateIndexBuffer(MemoryPtr ptr, uint32_t size) {
    log::core::Info("gfx::CreateIndexBuffer");
    return details::g_renderer.CreateIndexBuffer(std::move(ptr), size);
}

ShaderHandle CreateShader(const std::string &source, std::initializer_list<Attribute::Binding::Enum> in_types) {
    log::core::Info("gfx::CreateShader");
    return details::g_renderer.CreateShader(source, in_types);
}

TextureHandle CreateTexture(uint32_t width, uint32_t height, TextureFormat::Enum format, MemoryPtr ptr) {
    log::core::Info("gfx::CreateTexture");
    return CreateTexture(width, height, format,
                         {TextureWrap::Repeat, TextureWrap::Repeat, TextureWrap::Repeat},
                         {TextureFilter::Linear, TextureFilter::Linear, TextureFilter::Linear},
                         TextureFlags::None,
                         std::move(ptr));
}

TextureHandle CreateTexture(uint32_t width,
                            uint32_t height,
                            TextureFormat::Enum format,
                            TextureWrap wrap,
                            TextureFilter filter,
                            TextureFlags::Type flags,
                            MemoryPtr ptr) {
    log::core::Info("gfx::CreateTexture");
    return details::g_renderer.CreateTexture(std::move(ptr), width, height, format, wrap, filter, flags);
}

FrameBufferHandle CreateFrameBuffer(uint32_t width, uint32_t height, TextureFormat::Enum format, TextureWrap wrap) {
    return details::g_renderer.CreateFrameBuffer(width, height, format, wrap);
}

FrameBufferHandle CreateFrameBuffer(std::initializer_list<TextureHandle> textures) {
    return details::g_renderer.CreateFrameBuffer(textures, false);
}

void UpdateVertexBuffer(VertexBufferHandle handle, MemoryPtr ptr, uint32_t offset) {
    details::g_renderer.UpdateVertexBuffer(handle, std::move(ptr), offset);
}

void UpdateIndexBuffer(IndexBufferHandle handle, MemoryPtr ptr, uint32_t offset) {
    details::g_renderer.UpdateIndexBuffer(handle, std::move(ptr), offset);
}

void Render(CameraId camera_id, ShaderHandle shader_handle) {
    details::g_renderer.Draw(camera_id, shader_handle);
}

void Frame() {
    SPRINT_PROFILE_FUNCTION();
    details::g_renderer.RenderFrame();
}

void Destroy(VertexBufferHandle &handle) {
    log::core::Info("gfx::Destroy VertexBuffer");
    details::g_renderer.Destroy(handle);
}

void Destroy(IndexBufferHandle &handle) {
    log::core::Info("gfx::Destroy IndexBuffer");
    details::g_renderer.Destroy(handle);
}

void Destroy(FrameBufferHandle &handle) {
    log::core::Info("gfx::Destroy FrameBuffer");
    details::g_renderer.Destroy(handle);
}

void Destroy(TextureHandle &handle) {
    log::core::Info("gfx::Destroy Texture");
    details::g_renderer.Destroy(handle);
}

void Destroy(ShaderHandle &handle) {
    log::core::Info("gfx::Destroy Shader");
    details::g_renderer.Destroy(handle);
}

void SetTransform(const Matrix &matrix) {
    details::g_renderer.SetTransform(matrix);
}

void SetView(CameraId camera_id, const Matrix &matrix) {
    details::g_renderer.SetView(camera_id, matrix);
}

void SetViewRect(CameraId camera_id, const Rect &rect) {
    details::g_renderer.SetViewRect(camera_id, rect);
}

void SetProjection(CameraId camera_id, const Matrix &matrix) {
    details::g_renderer.SetProjection(camera_id, matrix);
}

void SetClear(CameraId camera_id, ClearFlag::Type clear_flag) {
    details::g_renderer.SetClear(camera_id, clear_flag);
}

void SetClearColor(CameraId camera_id, const Color &color) {
    details::g_renderer.SetClearColor(camera_id, color);
}

void SetUniform(ShaderHandle handle, const std::string &name, int value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string &name, bool value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string &name, float value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string &name, const Vec3 &value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string &name, const Vec4 &value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string &name, const Color &value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string &name, const Matrix &value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string &name, TextureHandle value, TexSlotId slot) {
    details::g_renderer.SetUniform(handle, name, value, slot);
}

MemoryPtr Copy(const void *data, uint32_t size) {
    std::shared_ptr<void> ptr(operator new(size), [](void *p) { operator delete(p); });
    memcpy(ptr.get(), data, size);
    return {ptr, size};
}

MemoryPtr MakeRef(void *data, uint32_t size) {
    std::shared_ptr<void> ptr(data, [](void *) {});
    return MemoryPtr{ptr, size};
}

void SetBuffer(VertexBufferHandle handle, uint32_t offset, uint32_t num) {
    details::g_renderer.SetBuffer(handle, offset, num);
}

void SetBuffer(IndexBufferHandle handle, uint32_t offset, uint32_t num) {
    details::g_renderer.SetBuffer(handle, offset, num);
}

void SetScissor(Rect rect) {
    details::g_renderer.SetScissor(rect);
}

void SetOptions(DrawConfig::Options options) {
    details::g_renderer.SetOptions(options);
}

void SetViewBuffer(CameraId camera_id, FrameBufferHandle handle) {
    details::g_renderer.SetViewBuffer(camera_id, handle);
}

void SetResolution(const Vec2Int &resolution) {
    details::g_config.resolution = resolution;
}

};

};