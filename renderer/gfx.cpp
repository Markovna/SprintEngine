#include "gfx.h"

#include <utility>
#include "gfx_details.h"
#include "Log.h"

namespace sprint {

namespace gfx {

void Init(Config config) {
    details::g_config = config;
    details::g_renderer.Init(details::g_config);
}

void Shutdown() {
    details::g_renderer.Shutdown();
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

VertexBufferHandle CreateVertexBuffer(const void *data, uint32_t size, VertexLayout layout) {
    return details::g_renderer.CreateVertexBuffer(data, size, layout);
}

IndexBufferHandle CreateIndexBuffer(const void *data, uint32_t size) {
    return details::g_renderer.CreateIndexBuffer(data, size);
}

ShaderHandle CreateShader(const std::string &source, std::initializer_list<Attribute::Binding::Enum> in_types) {
    return details::g_renderer.CreateShader(source, in_types);
}

TextureHandle CreateTexture(const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels) {
    return details::g_renderer.CreateTexture(data, width, height, channels);
}

void Render(CameraId camera_id, ShaderHandle shader_handle) {
    details::g_renderer.Draw(camera_id, shader_handle);
}

void Frame() {
    details::g_renderer.RenderFrame();
}

void SetBuffer(IndexBufferHandle handle) {
    details::g_renderer.SetBuffer(handle);
}

void SetBuffer(VertexBufferHandle handle) {
    details::g_renderer.SetBuffer(handle);
}

void Destroy(VertexBufferHandle& handle) {
    details::g_renderer.Destroy(handle);
}

void Destroy(IndexBufferHandle& handle) {
    details::g_renderer.Destroy(handle);
}

void Destroy(TextureHandle& handle) {
    details::g_renderer.Destroy(handle);
}

void Destroy(ShaderHandle& handle) {
    details::g_renderer.Destroy(handle);
}

void SetTransform(const Matrix& matrix) {
    details::g_renderer.SetTransform(matrix);
}

void SetView(CameraId camera_id, const Matrix& matrix) {
    details::g_renderer.SetView(camera_id, matrix);
}

void SetProjection(CameraId camera_id, const Matrix& matrix) {
    details::g_renderer.SetProjection(camera_id, matrix);
}

void SetClear(CameraId camera_id, ClearFlag clear_flag) {
    details::g_renderer.SetClear(camera_id, clear_flag);
}

void SetUniform(ShaderHandle handle, const std::string& name, int value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string& name, bool value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string& name, float value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string& name, const Vec3& value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string& name, const Vec4& value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string& name, const Color& value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string& name, const Matrix& value) {
    details::g_renderer.SetUniform(handle, name, value);
}

void SetUniform(ShaderHandle handle, const std::string& name, TextureHandle value) {
    details::g_renderer.SetUniform(handle, name, value);
}

}

}