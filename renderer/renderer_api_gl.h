#pragma once

#include "gfx_details.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace sprint {

namespace gfx {

namespace details {

class GLContext {
public:
    using WindowHandle = GLFWwindow*;

    explicit GLContext(void* handle) : handle_((WindowHandle) handle) {
        int w, h; glfwGetWindowSize(handle_, &w, &h);
        size_ = Vec2Int{ w, h };

        float x, y; glfwGetWindowContentScale(handle_, &x, &y);
        scale_ = Vec2{x, y};
        log::core::Info("New OpenGL context created. size: {}x{}, scale: {}x{}", w, h, x, y);
    }

    void MakeCurrent();
    void SwapBuffers();
    inline Vec2Int GetSize() const { return size_; }
    inline Vec2 GetScale() const { return scale_; }

    static void Init(GLContext& context);
    static GLContext CreateDefault(const Config& config);

private:
    WindowHandle handle_;
    Vec2Int size_;
    Vec2 scale_;
};

class GLRendererAPI : public RendererAPI {
private:
    struct VertexBuffer {
        VertexBuffer() noexcept : id(0), size(0), layout{} {};
        VertexBuffer(uint32_t id, uint32_t size, VertexLayout _layout) :
            id(id), size(size), layout(_layout)
        {}

        uint32_t id;
        uint32_t size;
        VertexLayout layout;
    };

    struct IndexBuffer {
        IndexBuffer() noexcept : id(0), size(0) {};
        IndexBuffer(uint32_t id, uint32_t size) noexcept : id(id), size(size) {}

        uint32_t id;
        uint32_t size;
    };

    class Shader {
    public:
        Shader() noexcept : attributes_mask_(0), id_(0) {}
        Shader(uint32_t id, const std::string& source, const Attribute::BindingPack& bindings);
        bool TryGetLocation(Attribute::Binding::Enum type, uint16_t& location) const;
        uint32_t get_id() const { return id_; }
    private:
        uint16_t attributes_mask_;
        uint16_t attribute_locations_[Attribute::Binding::Count]{};
        uint32_t id_;
    };

    struct Texture {
        Texture() noexcept : id(0) {}
        explicit Texture(uint32_t id) : id(id) {}

        uint32_t id;
    };

public:
    explicit GLRendererAPI(const Config& config);
    ~GLRendererAPI() override;

    void CreateVertexBuffer(VertexBufferHandle handle, const void* data, uint32_t data_size, uint32_t size, VertexLayout layout) override;
    void CreateIndexBuffer(IndexBufferHandle handle, const void* data, uint32_t data_size, uint32_t size) override;
    void CreateShader(ShaderHandle handle, const std::string& source, const Attribute::BindingPack& bindings) override;
    void CreateTexture(TextureHandle handle, const void* data, uint32_t data_size, uint32_t width, uint32_t height, uint32_t channels) override;

    void UpdateVertexBuffer(VertexBufferHandle handle, uint32_t offset, const void* data, uint32_t data_size) override;
    void UpdateIndexBuffer(IndexBufferHandle handle, uint32_t offset, const void* data, uint32_t data_sizer) override;

    void Destroy(VertexBufferHandle) override;
    void Destroy(IndexBufferHandle) override;
    void Destroy(ShaderHandle) override;
    void Destroy(TextureHandle) override;
    void Frame(const RendererContext* context, iterator_range<const DrawUnit*> draws) override;

private:
    void SetUniform(ShaderHandle, const std::string&, int);
    void SetUniform(ShaderHandle, const std::string&, bool);
    void SetUniform(ShaderHandle, const std::string&, float);
    void SetUniform(ShaderHandle, const std::string&, const Vec3&);
    void SetUniform(ShaderHandle, const std::string&, const Vec4&);
    void SetUniform(ShaderHandle, const std::string&, const Color&);
    void SetUniform(ShaderHandle, const std::string&, const Matrix&);
    void SetUniform(TextureHandle, int slot_idx);
    void Draw(const RendererContext*, const DrawUnit&);

    template <class TCommand>
    void ExecuteCommand(const TCommand& command);

    template <>
    void ExecuteCommand<SetUniformCommand>(const SetUniformCommand& command) {
        mpark::visit([this, &command](const auto& val){
                SetUniform(command.shader_handle, command.name, val);
            }, command.value);
    }

    template <>
    void ExecuteCommand<SetTextureCommand>(const SetTextureCommand& command) {
        SetUniform(command.texture_handle, command.slot);
    }

private:
    GLContext default_context_;
    uint32_t vao_ = 0;
    IndexBuffer index_buffers_[static_config::kIndexBuffersCapacity];
    VertexBuffer vertex_buffers_[static_config::kVertexBuffersCapacity];
    Shader shaders_[static_config::kShadersCapacity];
    Texture textures_[static_config::kTexturesCapacity];
};

}

}

}