#pragma once

#include "gfx_details.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace sprint {

namespace gfx {

namespace details {

struct TextureFormatInfo {
    GLint internal_format;
    GLenum format;
    GLenum type;
};

class GLContext {
public:
    using WindowHandle = GLFWwindow*;

    explicit GLContext(void* handle) : handle_((WindowHandle) handle) {
        log::core::Info("New OpenGL context created.");
    }

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
        uint32_t id{};
        uint32_t size{};
        VertexLayout layout{};
    };

    struct IndexBuffer {
        uint32_t id{};
        uint32_t size{};
    };

    struct FrameBuffer {
        uint32_t id{};
        texture_handle tex_handles[static_config::kFrameBufferMaxAttachments]{};
        uint32_t tex_num{};
        bool destroy_tex{};
    };

    struct Shader {
    public:
        bool TryGetLocation(Attribute::Binding::Enum type, uint16_t& location) const;

        uint32_t model_location = 0;
        uint32_t view_location = 0;
        uint32_t proj_location = 0;
        uint16_t enabled_attributes_mask = 0;
        uint16_t attributes_mask = 0;
        uint16_t attribute_locations[Attribute::Binding::Count]{};
        uint32_t id = 0;
    };

    struct Texture {
        uint32_t id{};
        bool render_buffer{};
        TextureFormat::Enum format{};
    };

    struct Uniform {
        uint32_t location;
        shader_handle shader;
    };

public:
    explicit GLRendererAPI(const Config& config);
    ~GLRendererAPI() override;

    void CreateVertexBuffer(vertexbuf_handle handle, const void* data, uint32_t data_size, uint32_t size, VertexLayout layout) override;
    void CreateIndexBuffer(indexbuf_handle handle, const void* data, uint32_t data_size, uint32_t size) override;
    void CreateFrameBuffer(framebuf_handle handle, texture_handle*, uint32_t num, bool destroy_tex = false) override;
    void CreateShader(shader_handle handle, const std::string& source, const Attribute::BindingPack& bindings) override;
    void CreateUniform(uniform_handle, shader_handle, char* name, uint16_t size) override;
    void CreateTexture(texture_handle handle, const void* data, uint32_t data_size, uint32_t width, uint32_t height,
                       TextureFormat::Enum, TextureWrap wrap, TextureFilter filter, TextureFlags::Type flags) override;

    void UpdateVertexBuffer(vertexbuf_handle handle, uint32_t offset, const void* data, uint32_t data_size) override;
    void UpdateIndexBuffer(indexbuf_handle handle, uint32_t offset, const void* data, uint32_t data_sizer) override;

    void Destroy(vertexbuf_handle) override;
    void Destroy(indexbuf_handle) override;
    void Destroy(framebuf_handle) override;
    void Destroy(shader_handle) override;
    void Destroy(uniform_handle) override;
    void Destroy(texture_handle) override;
    void RenderFrame(const Frame&) override;

private:
    void SetUniform(uniform_handle, int);
    void SetUniform(uniform_handle, bool);
    void SetUniform(uniform_handle, float);
    void SetUniform(uniform_handle, const Vec3&);
    void SetUniform(uniform_handle, const Vec4&);
    void SetUniform(uniform_handle, const Color&);
    void SetUniform(uniform_handle, const Matrix&);

//    void SetUniform(shader_handle, const std::string&, int);
//    void SetUniform(shader_handle, const std::string&, bool);
//    void SetUniform(shader_handle, const std::string&, float);
//    void SetUniform(shader_handle, const std::string&, const Vec3&);
//    void SetUniform(shader_handle, const std::string&, const Vec4&);
//    void SetUniform(shader_handle, const std::string&, const Color&);
//    void SetUniform(shader_handle, const std::string&, const Matrix&);
    void SetUniform(texture_handle, int slot_idx);

    template<class T> void ExecuteCommand(const T&) {}

private:
    GLContext default_context_;
    uint32_t vao_ = 0;
    IndexBuffer index_buffers_[static_config::kIndexBuffersCapacity];
    VertexBuffer vertex_buffers_[static_config::kVertexBuffersCapacity];
    FrameBuffer frame_buffers_[static_config::kVertexBuffersCapacity];
    Shader shaders_[static_config::kShadersCapacity];
    Texture textures_[static_config::kTexturesCapacity];
    Uniform uniforms_[static_config::kUniformsCapacity];
};

}

}

}