#include "gfx.h"

#include <utility>
#include "gfx_details.h"
#include "core/log.h"

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

const TextureFormat::Info TextureFormat::formats[] = {
    { sizeof(uint8_t),    1,   0,   0 }, // R8
    { sizeof(uint8_t),    3,   0,   0 }, // RGB8
    { sizeof(uint8_t),    4,   0,   0 }, // RGBA8
    { 0,    0,   24,  8 }, // D24S8
};

vertexbuf_handle CreateVertexBuffer(MemoryPtr ptr, uint32_t size, VertexLayout layout) {
    log::core::Info("gfx::CreateVertexBuffer");
    return details::g_renderer.CreateVertexBuffer(std::move(ptr), size, layout);
}

indexbuf_handle CreateIndexBuffer(MemoryPtr ptr, uint32_t size) {
    log::core::Info("gfx::CreateIndexBuffer");
    return details::g_renderer.CreateIndexBuffer(std::move(ptr), size);
}

shader_handle CreateShader(const std::string &source) {
    log::core::Info("gfx::CreateShader");
    return details::g_renderer.CreateShader(source);
}

texture_handle CreateTexture(uint32_t width, uint32_t height, TextureFormat::Enum format, MemoryPtr ptr) {
    log::core::Info("gfx::CreateTexture");
    return CreateTexture(width, height, format,
                         {TextureWrap::Repeat, TextureWrap::Repeat, TextureWrap::Repeat},
                         {TextureFilter::Linear, TextureFilter::Linear, TextureFilter::Linear},
                         TextureFlags::None,
                         std::move(ptr));
}

texture_handle CreateTexture(uint32_t width,
                             uint32_t height,
                             TextureFormat::Enum format,
                             TextureWrap wrap,
                             TextureFilter filter,
                             TextureFlags::Type flags,
                             MemoryPtr ptr) {
    log::core::Info("gfx::CreateTexture");
    return details::g_renderer.CreateTexture(std::move(ptr), width, height, format, wrap, filter, flags);
}

framebuf_handle CreateFrameBuffer(std::initializer_list<texture_handle> textures) {
    return details::g_renderer.CreateFrameBuffer(textures, false);
}

uniform_handle CreateUniform(const char* c_str) {
    return details::g_renderer.CreateUniform(c_str);
}

void UpdateVertexBuffer(vertexbuf_handle handle, MemoryPtr ptr, uint32_t offset) {
    details::g_renderer.UpdateVertexBuffer(handle, std::move(ptr), offset);
}

void UpdateIndexBuffer(indexbuf_handle handle, MemoryPtr ptr, uint32_t offset) {
    details::g_renderer.UpdateIndexBuffer(handle, std::move(ptr), offset);
}

void Render(CameraId camera_id, shader_handle shader_handle) {
    details::g_renderer.Draw(camera_id, shader_handle);
}

void Frame() {
    details::g_renderer.RenderFrame();
}

void Destroy(vertexbuf_handle &handle) {
    log::core::Info("gfx::Destroy VertexBuffer");
    details::g_renderer.Destroy(handle);
}

void Destroy(indexbuf_handle &handle) {
    log::core::Info("gfx::Destroy IndexBuffer");
    details::g_renderer.Destroy(handle);
}

void Destroy(framebuf_handle &handle) {
    log::core::Info("gfx::Destroy FrameBuffer");
    details::g_renderer.Destroy(handle);
}

void Destroy(texture_handle &handle) {
    log::core::Info("gfx::Destroy Texture");
    details::g_renderer.Destroy(handle);
}

void Destroy(shader_handle &handle) {
    log::core::Info("gfx::Destroy Shader");
    details::g_renderer.Destroy(handle);
}

void Destroy(uniform_handle& handle) {
    log::core::Info("gfx::Destroy Uniform");
    details::g_renderer.Destroy(handle);
}

bool IsValid(vertexbuf_handle handle) {
    return details::g_renderer.IsValid(handle);
}

bool IsValid(indexbuf_handle handle) {
    return details::g_renderer.IsValid(handle);
}

bool IsValid(framebuf_handle handle) {
    return details::g_renderer.IsValid(handle);
}

bool IsValid(shader_handle handle) {
    return details::g_renderer.IsValid(handle);
}

bool IsValid(uniform_handle handle) {
    return details::g_renderer.IsValid(handle);
}

bool IsValid(texture_handle handle) {
    return details::g_renderer.IsValid(handle);
}

void SetTransform(const Matrix &matrix) {
    details::g_renderer.SetTransform(matrix);
}

void SetView(CameraId camera_id, const Matrix &matrix) {
    details::g_renderer.SetView(camera_id, matrix);
}

void SetViewRect(CameraId camera_id, const RectInt &rect) {
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

void SetUniform(uniform_handle handle, int value) {
    details::g_renderer.SetUniform(handle, value);
}

void SetUniform(uniform_handle handle, bool value) {
    details::g_renderer.SetUniform(handle, value);
}

void SetUniform(uniform_handle handle, float value) {
    details::g_renderer.SetUniform(handle, value);
}

void SetUniform(uniform_handle handle, const Vec3 &value) {
    details::g_renderer.SetUniform(handle, value);
}

void SetUniform(uniform_handle handle, const Vec4 &value) {
    details::g_renderer.SetUniform(handle, value);
}

void SetUniform(uniform_handle handle, const Color &value) {
    details::g_renderer.SetUniform(handle, value);
}

void SetUniform(uniform_handle handle, const Matrix &value) {
    details::g_renderer.SetUniform(handle, value);
}

void SetUniform(uniform_handle handle, texture_handle value, TexSlotId slot) {
    details::g_renderer.SetUniform(handle, value, slot);
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

void SetBuffer(vertexbuf_handle handle, uint32_t offset, uint32_t num) {
    details::g_renderer.SetBuffer(handle, offset, num);
}

void SetBuffer(indexbuf_handle handle, uint32_t offset, uint32_t num) {
    details::g_renderer.SetBuffer(handle, offset, num);
}

void SetScissor(RectInt rect) {
    details::g_renderer.SetScissor(rect);
}

void SetOptions(DrawConfig::Options options) {
    details::g_renderer.SetOptions(options);
}

void SetViewBuffer(CameraId camera_id, framebuf_handle handle) {
    details::g_renderer.SetViewBuffer(camera_id, handle);
}

void SetResolution(const Vec2Int &resolution) {
    details::g_config.resolution = resolution;
}

};

};