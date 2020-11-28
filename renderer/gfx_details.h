#pragma once

#include "gfx.h"
#include "Log.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <IdAllocator.h>

namespace sprint {

namespace gfx {

namespace details {

static Config g_config;

class RendererAPI {
public:
    RendererAPI() = default;
    RendererAPI(const RendererAPI&) = delete;
    RendererAPI(RendererAPI&&) = delete;

    RendererAPI& operator=(const RendererAPI&) = delete;
    RendererAPI& operator=(RendererAPI&&) = delete;

    virtual ~RendererAPI() = default;

    virtual void CreateVertexBuffer(VertexBufferHandle, const void* data, uint32_t size, VertexLayout layout) = 0;
    virtual void CreateIndexBuffer(IndexBufferHandle, const void* data, uint32_t size) = 0;
    virtual void CreateShader(ShaderHandle, const std::string& source, std::initializer_list<Attribute::Binding::Enum> in_types) = 0;
    virtual void CreateTexture(TextureHandle, const uint8_t* data, uint32_t width, uint32_t height, uint32_t channels) = 0;

    virtual void Destroy(VertexBufferHandle) = 0;
    virtual void Destroy(IndexBufferHandle) = 0;
    virtual void Destroy(ShaderHandle) = 0;
    virtual void Destroy(TextureHandle) = 0;

    virtual void SetUniform(ShaderHandle, const std::string&, int) = 0;
    virtual void SetUniform(ShaderHandle, const std::string&, bool) = 0;
    virtual void SetUniform(ShaderHandle, const std::string&, float) = 0;
    virtual void SetUniform(ShaderHandle, const std::string&, const Vec3&) = 0;
    virtual void SetUniform(ShaderHandle, const std::string&, const Vec4&) = 0;
    virtual void SetUniform(ShaderHandle, const std::string&, const Color&) = 0;
    virtual void SetUniform(ShaderHandle, const std::string&, const Matrix&) = 0;
    virtual void SetUniform(ShaderHandle, const std::string&, TextureHandle, int slot_idx) = 0;
};

std::unique_ptr<RendererAPI> CreateRendererApi(const Config&);

struct UniformData {
    union Data {
        int int_value;
        bool bool_value;
        float float_value;
        Vec3 vec3_value;
        Vec4 vec4_value;
        Color color_value;
        Matrix matrix_value;
    };


};

struct Camera {
    Matrix view;
    Matrix projection;
    ClearFlag clear_flag;
};

struct DrawUnit {
    Matrix transform;
    IndexBufferHandle ib_handle;
    VertexBufferHandle vb_handle;
    ShaderHandle shader_handle;
    CameraId camera_id = UINT16_MAX;

};

class Frame {
public:
    DrawUnit& GetDraw() {
        assert(draws_count_ < static_config::kMaxDrawCallsCount);
        return draws_[draws_count_];
    }

    void Next() {
        draws_count_++;
    }

    void Reset() {
        draws_count_ = 0;
    }

private:
    DrawUnit draws_[static_config::kMaxDrawCallsCount];
    uint32_t draws_count_ = 0;
};

class Renderer {
public:
    Renderer() noexcept = default;

    void Init(const Config& config) {
        assert(!api_);
        api_ = CreateRendererApi(config);
    }

    void Shutdown() {
        assert(api_);
        api_.reset();
    }

    void RenderFrame() {

    }

    void Draw(CameraId camera_id, ShaderHandle shader_handle) {
        DrawUnit& draw = frame_.GetDraw();
        draw.camera_id = camera_id;
        draw.shader_handle = shader_handle;

        frame_.Next();
    }

    VertexBufferHandle CreateVertexBuffer(const void* data, uint32_t size, VertexLayout layout) {
        VertexBufferHandle handle(vertex_buffer_handles_.Alloc());
        //TODO
        return handle;
    }

    IndexBufferHandle CreateIndexBuffer(const void* data, uint32_t size) {
        IndexBufferHandle handle(index_buffer_handles_.Alloc());
        //TODO
        return handle;
    }

    ShaderHandle CreateShader(const std::string &source, std::initializer_list<Attribute::Binding::Enum> in_types) {
        ShaderHandle handle(shader_handles_.Alloc());
        //TODO
        return handle;
    }

    TextureHandle CreateTexture(const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels) {
        TextureHandle handle(texture_handles_.Alloc());
        //TODO
        return handle;
    }

    void Destroy(VertexBufferHandle& handle) {
        assert(handle.IsValid() && "Renderer::Destroy failed: Invalid vertex buffer handle");

        //TODO
        vertex_buffer_handles_.Free(handle.ID);
        handle = VertexBufferHandle::Invalid;
    }

    void Destroy(IndexBufferHandle& handle) {
        assert(handle.IsValid() && "Renderer::Destroy failed: Invalid index buffer handle");

        //TODO
        index_buffer_handles_.Free(handle.ID);
        handle = IndexBufferHandle::Invalid;
    }

    void Destroy(TextureHandle& handle) {
        assert(handle.IsValid() && "Renderer::Destroy failed: Invalid texture handle");

        //TODO
        texture_handles_.Free(handle.ID);
        handle = TextureHandle::Invalid;
    }

    void Destroy(ShaderHandle& handle) {
        assert(handle.IsValid() && "Renderer::Destroy failed: Invalid shader handle");

        //TODO
        shader_handles_.Free(handle.ID);
        handle = ShaderHandle::Invalid;
    }

    void SetUniform(ShaderHandle handle, const std::string& name, int value) {

    }

    void SetUniform(ShaderHandle handle, const std::string& name, bool value) {

    }

    void SetUniform(ShaderHandle handle, const std::string& name, float value) {

    }

    void SetUniform(ShaderHandle handle, const std::string& name, const Vec3& value) {

    }

    void SetUniform(ShaderHandle handle, const std::string& name, const Vec4& value) {

    }

    void SetUniform(ShaderHandle handle, const std::string& name, const Color& value) {

    }

    void SetUniform(ShaderHandle handle, const std::string& name, const Matrix& value) {

    }

    void SetUniform(ShaderHandle handle, const std::string& name, TextureHandle value) {

    }

    void SetBuffer(IndexBufferHandle handle) {
        DrawUnit& draw = frame_.GetDraw();
        draw.ib_handle = handle;
    }

    void SetBuffer(VertexBufferHandle handle) {
        DrawUnit& draw = frame_.GetDraw();
        draw.vb_handle = handle;
    }

    void SetTransform(const Matrix& value) {
        DrawUnit& draw = frame_.GetDraw();
        draw.transform = value;
    }

    void SetView(CameraId id, const Matrix& value) {
        cameras_[id].view = value;
    }

    void SetProjection(CameraId id, const Matrix& value) {
        cameras_[id].projection = value;
    }

    void SetClear(CameraId id, ClearFlag flag) {
        cameras_[id].clear_flag = flag;
    }

private:
    std::unique_ptr<RendererAPI> api_;

    template<size_t Size> using handle_alloc = IdAllocator<Size>;

    handle_alloc<static_config::kIndexBuffersCapacity> index_buffer_handles_;
    handle_alloc<static_config::kVertexBuffersCapacity> vertex_buffer_handles_;
    handle_alloc<static_config::kShadersCapacity> shader_handles_;
    handle_alloc<static_config::kTexturesCapacity> texture_handles_;

    Camera cameras_[static_config::kCamerasCapacity];
    Frame frame_;
};

static Renderer g_renderer;

}

}

}
