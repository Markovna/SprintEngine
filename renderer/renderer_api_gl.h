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

    explicit GLContext(void* handle) : handle_((WindowHandle) handle) {}

    void MakeCurrent();
    void SwapBuffers();

    static void Init(GLContext& context);
    static GLContext CreateDefault(const Config& config);

private:
    WindowHandle handle_;
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
        Shader(uint32_t id, const std::string& source, std::initializer_list<Attribute::Binding::Enum> bindings);
        bool TryGetLocation(Attribute::Binding::Enum type, uint32_t& location) const;
        uint32_t get_id() const { return id_; }
    private:
        uint32_t attributes_mask_;
        uint32_t attribute_locations_[Attribute::Binding::Count]{};
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

    void CreateVertexBuffer(VertexBufferHandle handle, const void* data, uint32_t size, VertexLayout layout) override;
    void CreateIndexBuffer(IndexBufferHandle handle, const void* data, uint32_t size) override;
    void CreateShader(ShaderHandle handle, const std::string& source, std::initializer_list<Attribute::Binding::Enum> in_types) override;
    void CreateTexture(TextureHandle handle, const uint8_t* data, uint32_t width, uint32_t height, uint32_t channels) override;

    void Destroy(VertexBufferHandle) override;
    void Destroy(IndexBufferHandle) override;
    void Destroy(ShaderHandle) override;
    void Destroy(TextureHandle) override;

    void SetUniform(ShaderHandle, const std::string&, int) override;
    void SetUniform(ShaderHandle, const std::string&, bool) override;
    void SetUniform(ShaderHandle, const std::string&, float) override;
    void SetUniform(ShaderHandle, const std::string&, const Vec3&) override;
    void SetUniform(ShaderHandle, const std::string&, const Vec4&) override;
    void SetUniform(ShaderHandle, const std::string&, const Color&) override;
    void SetUniform(ShaderHandle, const std::string&, const Matrix&) override;
    void SetUniform(ShaderHandle, const std::string&, TextureHandle, int slot_idx) override;

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