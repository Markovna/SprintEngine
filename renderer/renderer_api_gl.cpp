#include "renderer_api_gl.h"
#include "Log.h"
#include "shader_utils.h"

namespace sprint::gfx::details {

#define RENDERER_API_DEBUG 1

#if RENDERER_API_DEBUG
#	define CHECK_ERRORS(__func) _CHECK_GL_ERRORS(__func)
#else
#   define CHECK_ERRORS(__func) __func
#endif // RENDERER_API_DEBUG

#define _CHECK_GL_ERRORS(__func) \
    __func;                            \
    CheckErrors(log::Format("\n{0}:{1}", __FILE__, __LINE__))

static void CheckErrors(const std::string_view msg) {
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        log::core::Error("GLRendererAPI error #{0}: {1}", err, msg);
    }
}

static void CheckCompileStatus(uint32_t shaderID) {
    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        log::core::Error("GL::Shader compilation failed with errors:\n{}", infoLog);
    }
}

static void CheckLinkStatus(uint32_t shaderID) {
    int success;
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
        log::core::Error("GL::Shader linking failed with errors:\n{}", infoLog);
    }
}

static GLenum GetShaderType(ShaderType::Enum type) {
    static GLenum shader_types[] = {
        GL_VERTEX_SHADER, GL_FRAGMENT_SHADER
    };
    return shader_types[type];
}

static uint32_t CompileShader(ShaderType::Enum type, const std::string& source) {
    uint32_t shaderID = glCreateShader(GetShaderType(type));
    const GLchar* sourceCStr = source.c_str();
    CHECK_ERRORS(glShaderSource(shaderID, 1, &sourceCStr, NULL));
    CHECK_ERRORS(glCompileShader(shaderID));
    CheckCompileStatus(shaderID);
    return shaderID;
}

static GLenum GetWrap(TextureWrap::Enum type) {
    static GLenum wraps[] = {
        GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
    };
    return wraps[type];
}

static std::tuple<GLenum, GLenum, GLenum> GetWrap(const TextureWrap& type) {
   return std::make_tuple(GetWrap(type.u), GetWrap(type.v), GetWrap(type.w));
}

static GLenum GetMagFilter(TextureFilter::Enum type) {
    static GLenum filters[] = {
        GL_LINEAR, GL_NEAREST
    };
    return filters[type];
}

static GLenum GetMinFilter(TextureFilter::Enum min, TextureFilter::Enum map, bool has_mipmaps) {
    static GLenum filters[][3] = {
        { GL_LINEAR, GL_LINEAR_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_LINEAR},
        { GL_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_NEAREST}
    };

    return filters[min][has_mipmaps ? map : 0];
}

static std::tuple<GLenum, GLenum> GetFilters(const TextureFilter& type, bool has_mipmaps) {
    return std::make_tuple( GetMinFilter(type.min, type.mag, has_mipmaps), GetMagFilter(type.mag));
}

GLRendererAPI::Shader::Shader(uint32_t id, const std::string &source, const Attribute::BindingPack& bindings)
        : id_(id) {
    std::string shaders[ShaderType::Count];
    PreProcess(source, shaders);
    for (size_t i = 0; i < ShaderType::Count; i++) {
        auto shader_id = CompileShader((ShaderType::Enum) i, shaders[i]);
        glAttachShader(id_, shader_id);
        glDeleteShader(shader_id);
    }

    CHECK_ERRORS(glLinkProgram(id_));
    CheckLinkStatus(id_);

    attributes_mask_ = bindings.mask;
    std::memcpy(attribute_locations_, bindings.locations, Attribute::Binding::Count);
}

bool GLRendererAPI::Shader::TryGetLocation(Attribute::Binding::Enum type, uint16_t& location) const {
    if (attributes_mask_ & (1u << type)) {
        location = attribute_locations_[type];
        return true;
    }
    return false;
}

void GLRendererAPI::CreateVertexBuffer(VertexBufferHandle handle, const void* data, uint32_t data_size, uint32_t size, VertexLayout layout) {
    uint32_t bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    CHECK_ERRORS(glBufferData(GL_ARRAY_BUFFER, data_size, data, data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    vertex_buffers_[handle.ID] = VertexBuffer(bufferId, size, layout);
}

void GLRendererAPI::CreateIndexBuffer(IndexBufferHandle handle, const void* data, uint32_t data_size, uint32_t size) {
    uint32_t bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
    CHECK_ERRORS(glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, data, data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    index_buffers_[handle.ID] = IndexBuffer(bufferId, size);
}

void GLRendererAPI::CreateFrameBuffer(FrameBufferHandle handle, TextureHandle* handles, uint32_t num, bool destroy_tex) {
    uint32_t bufferId;
    glGenFramebuffers(1, &bufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, bufferId);

    for (int i = 0; i < num; i++) {
        TextureHandle tex = handles[i];
        uint32_t tex_id = textures_[tex.ID].id;
        TextureFormat::Enum format = textures_[tex.ID].format;
        GLenum attachment;

        bool is_depth = TextureFormat::IsDepth(format);
        if (is_depth) {
            uint8_t stencil_bits = TextureFormat::GetStencilBits(format);
            uint8_t depth_bits = TextureFormat::GetDepthBits(format);
            if (stencil_bits && depth_bits) {
                attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            } else if (depth_bits) {
                attachment = GL_DEPTH_ATTACHMENT;
            } else {
                attachment = GL_STENCIL_ATTACHMENT;
            }
        } else {
            attachment = GL_COLOR_ATTACHMENT0;
        }

        if (textures_[tex.ID].render_buffer) {
            CHECK_ERRORS(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, tex_id));
        } else {
            CHECK_ERRORS(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex_id, 0));
        }

        frame_buffers_[handle.ID].tex_handles[i] = tex;
    }

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    frame_buffers_[handle.ID].id = bufferId;
    frame_buffers_[handle.ID].tex_num = num;
    frame_buffers_[handle.ID].destroy_tex = destroy_tex;
}

void GLRendererAPI::UpdateVertexBuffer(VertexBufferHandle handle, uint32_t offset, const void* data, uint32_t data_size) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers_[handle.ID].id);
    CHECK_ERRORS(glBufferSubData(GL_ARRAY_BUFFER, offset, data_size, data));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLRendererAPI::UpdateIndexBuffer(IndexBufferHandle handle, uint32_t offset, const void* data, uint32_t data_size) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers_[handle.ID].id);
    CHECK_ERRORS(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, data_size, data));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLRendererAPI::CreateShader(ShaderHandle handle,
                                 const std::string &source,
                                 const Attribute::BindingPack& bindings) {
    shaders_[handle.ID] = Shader(glCreateProgram(), source, bindings);
}

static const TextureFormatInfo& GetTextureFormat(TextureFormat::Enum format) {
    static TextureFormatInfo texture_formats[] =
        {
            { GL_RED,                   GL_RED,                     GL_UNSIGNED_BYTE },     // R8
            { GL_RGB,                   GL_RGB,                     GL_UNSIGNED_BYTE },     // RGB8
            { GL_RGBA,                  GL_RGBA,                    GL_UNSIGNED_BYTE },     // RGBA8
            { GL_DEPTH24_STENCIL8,      GL_DEPTH_STENCIL,           GL_UNSIGNED_INT_24_8 }, // D24S8
        };

    return texture_formats[format];
}

void GLRendererAPI::CreateTexture(TextureHandle handle,
                                  const void* data, uint32_t data_size,
                                  uint32_t width, uint32_t height,
                                  TextureFormat::Enum format, TextureWrap wrap, TextureFilter filter, TextureFlags::Type flags) {
    uint32_t textureId;
    bool render_buffer = (flags & TextureFlags::RenderTarget) != 0;
    const TextureFormatInfo& format_info = GetTextureFormat(format);

    if (render_buffer) {
        CHECK_ERRORS(glGenRenderbuffers(1, &textureId));
        CHECK_ERRORS(glBindRenderbuffer(GL_RENDERBUFFER, textureId));

        CHECK_ERRORS(glRenderbufferStorage(GL_RENDERBUFFER, format_info.internal_format, width, height));

        CHECK_ERRORS(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    } else {
        CHECK_ERRORS(glGenTextures(1, &textureId));
        CHECK_ERRORS(glBindTexture(GL_TEXTURE_2D, textureId));

        auto [s_wrap, t_wrap, r_wrap] = GetWrap(wrap);
        CHECK_ERRORS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s_wrap));
        CHECK_ERRORS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t_wrap));
        CHECK_ERRORS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, r_wrap));

        auto [min_filter, mag_filter] = GetFilters(filter, true);
        CHECK_ERRORS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter));
        CHECK_ERRORS(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter));

        CHECK_ERRORS(glTexImage2D(GL_TEXTURE_2D, 0, format_info.internal_format, width, height, 0, format_info.format, format_info.type, data));

        CHECK_ERRORS(glGenerateMipmap(GL_TEXTURE_2D));

        CHECK_ERRORS(glBindTexture(GL_TEXTURE_2D, 0));
    }

    textures_[handle.ID].id = textureId;
    textures_[handle.ID].render_buffer = render_buffer;
    textures_[handle.ID].format = format;
}

GLRendererAPI::~GLRendererAPI() {
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao_);
}

GLRendererAPI::GLRendererAPI(const Config &config)
        : default_context_(GLContext::CreateDefault(config)) {

    GLContext::Init(default_context_);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    glEnable(GL_DEPTH_TEST);

    /**/

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
//    glDisable(GL_DEPTH_TEST);
//    glEnable(GL_SCISSOR_TEST);

     /**/
}

void GLRendererAPI::Destroy(VertexBufferHandle handle) {
    glDeleteBuffers(1, &vertex_buffers_[handle.ID].id);
}

void GLRendererAPI::Destroy(IndexBufferHandle handle) {
    glDeleteBuffers(1, &index_buffers_[handle.ID].id);
}

void GLRendererAPI::Destroy(FrameBufferHandle handle) {

    if (frame_buffers_[handle.ID].destroy_tex) {
        for (int i = 0; i < frame_buffers_[handle.ID].tex_num; i++) {
            Destroy(frame_buffers_[handle.ID].tex_handles[i]);
        }
    }

    glDeleteFramebuffers(1, &frame_buffers_[handle.ID].id);
}

void GLRendererAPI::Destroy(ShaderHandle handle) {
    glDeleteProgram(shaders_[handle.ID].get_id());
}

void GLRendererAPI::Destroy(TextureHandle handle) {
    glDeleteTextures(1, &textures_[handle.ID].id);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, int value) {
    CHECK_ERRORS(glUseProgram(shaders_[handle.ID].get_id()));
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    CHECK_ERRORS(glUniform1i(location, value));
    glUseProgram(0);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, bool value) {
    CHECK_ERRORS(glUseProgram(shaders_[handle.ID].get_id()));
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    CHECK_ERRORS(glUniform1i(location, value));
    glUseProgram(0);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, float value) {
    CHECK_ERRORS(glUseProgram(shaders_[handle.ID].get_id()));
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    CHECK_ERRORS(glUniform1f(location, value));
    glUseProgram(0);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, const Vec3& value) {
    CHECK_ERRORS(glUseProgram(shaders_[handle.ID].get_id()));
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    CHECK_ERRORS(glUniform3f(location, value.x, value.y, value.z));
    glUseProgram(0);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, const Vec4& value) {
    CHECK_ERRORS(glUseProgram(shaders_[handle.ID].get_id()));
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    CHECK_ERRORS(glUniform4f(location, value.x, value.y, value.z, value.w));
    glUseProgram(0);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, const Color& value) {
    CHECK_ERRORS(glUseProgram(shaders_[handle.ID].get_id()));
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    CHECK_ERRORS(glUniform4f(location, value.r, value.g, value.b, value.a));
    glUseProgram(0);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, const Matrix& value) {
    CHECK_ERRORS(glUseProgram(shaders_[handle.ID].get_id()));
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    CHECK_ERRORS(glUniformMatrix4fv(location, 1, GL_FALSE, (float*) &value));
    glUseProgram(0);
}

void GLRendererAPI::SetUniform(TextureHandle handle, int slot_idx) {
    glActiveTexture(GL_TEXTURE0 + slot_idx);
    CHECK_ERRORS(glBindTexture(GL_TEXTURE_2D, textures_[handle.ID].id));
}

template<> void GLRendererAPI::ExecuteCommand(const SetUniformCommand& command) {
    mpark::visit([this, &command](const auto& val){
        SetUniform(command.shader_handle, command.name, val);
    }, command.value);
}

template<> void GLRendererAPI::ExecuteCommand(const SetTextureCommand& command) {
    SetUniform(command.texture_handle, command.slot);
}

static GLenum ToType(Attribute::Type type) {
    switch (type) {
        case Attribute::Type::FLOAT: return GL_FLOAT;
        case Attribute::Type::UINT: return GL_UNSIGNED_INT;
        case Attribute::Type::INT: return GL_INT;
        case Attribute::Type::BYTE: return GL_UNSIGNED_BYTE;
    }
    assert("You are not supposed to be here");
}

static GLbitfield ToGLBits(ClearFlagMask mask) {
    GLbitfield bitfield = 0;
    if (mask & ClearFlag::Depth) bitfield |= (uint32_t) GL_DEPTH_BUFFER_BIT;
    if (mask & ClearFlag::Color) bitfield |= (uint32_t) GL_COLOR_BUFFER_BIT;
    if (mask & ClearFlag::Stencil) bitfield |= (uint32_t) GL_STENCIL_BUFFER_BIT;
    return bitfield;
}

void GLRendererAPI::Frame(const RendererContext* context, iterator_range<const DrawUnit*> draws) {

    default_context_.MakeCurrent();

    CHECK_ERRORS(glDisable(GL_SCISSOR_TEST));
    for (auto& camera : context->cameras) {
        if (camera.frame_buffer.IsValid()) {
            CHECK_ERRORS(glBindFramebuffer(GL_FRAMEBUFFER, frame_buffers_[camera.frame_buffer.ID].id));
        }

        CHECK_ERRORS(glClearColor(camera.clear_color.r, camera.clear_color.g, camera.clear_color.b, camera.clear_color.a));
        CHECK_ERRORS(glClear(ToGLBits(camera.clear_flag)));

        if (camera.frame_buffer.IsValid()) {
            CHECK_ERRORS(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }
    }

    for (const DrawUnit& draw : draws) {
        Draw(context, draw);
    }

    default_context_.SwapBuffers();
}

static void DisableAttributes(uint16_t mask) {
    while (mask) {
        uint16_t index = __builtin_ctzl(mask);
        CHECK_ERRORS(glDisableVertexAttribArray(index));
        mask ^= 1u << index;
    }
}

static void SetDepthTest(bool enable) {
    if (enable) {
        CHECK_ERRORS(glEnable(GL_DEPTH_TEST));
    } else {
        CHECK_ERRORS(glDisable(GL_DEPTH_TEST));
    }
}

static void SetViewport(Rect rect, Vec2Int resolution) {
    CHECK_ERRORS(glViewport(
        rect.x,
        resolution.y - (rect.y + rect.height),
        rect.width,
        rect.height
    ));
}

static void SetScissor(Rect scissor, Vec2Int size) {
    if (Valid(scissor)) {
        CHECK_ERRORS(glEnable(GL_SCISSOR_TEST));
        CHECK_ERRORS(glScissor(
            scissor.x,
            size.y - (scissor.y + scissor.height),
            scissor.width,
            scissor.height
        ));
    }
    else {
        CHECK_ERRORS(glDisable(GL_SCISSOR_TEST));
    }
}

void GLRendererAPI::Draw(const RendererContext* context, const DrawUnit& draw) {
    for (const auto& command : draw.command_buffer) {
        mpark::visit([this](const auto& c){
                ExecuteCommand(c);
            }, command);
    }

    const Camera& camera = context->cameras[draw.camera_id];

    if (camera.frame_buffer != FrameBufferHandle::Invalid) {
        CHECK_ERRORS(glBindFramebuffer(GL_FRAMEBUFFER, frame_buffers_[camera.frame_buffer.ID].id));
    }


    SetViewport(camera.viewport, context->resolution);

    SetUniform(draw.shader_handle, "model", draw.transform);
    SetUniform(draw.shader_handle, "view", camera.view);
    SetUniform(draw.shader_handle, "projection", camera.projection);

    SetScissor(draw.scissor, context->resolution);
    SetDepthTest(draw.options & DrawConfig::DEPTH_TEST);

    if (draw.vb_handle.IsValid()) {
        VertexBuffer& vb = vertex_buffers_[draw.vb_handle.ID];
        Shader& shader = shaders_[draw.shader_handle.ID];

        CHECK_ERRORS(glBindBuffer(GL_ARRAY_BUFFER, vb.id));

        CHECK_ERRORS(glUseProgram(shader.get_id()));
        uint16_t enabledAttribMask = 0;
        uint32_t base_vertex = draw.vb_offset * vb.layout.get_stride();
        for (const auto& item : vb.layout) {
            uint16_t idx;
            if (shader.TryGetLocation(item.attribute.binding, idx)) {
                CHECK_ERRORS(glVertexAttribPointer(
                    idx,
                    item.attribute.format.size,
                    ToType(item.attribute.format.type),
                    item.attribute.normalized,
                    vb.layout.get_stride(),
                    (void*)(base_vertex + item.offset)
                ));
                CHECK_ERRORS(glEnableVertexAttribArray(idx));
                enabledAttribMask |= (1u << idx);
            }
        }
        DisableAttributes(~enabledAttribMask);

        if (draw.ib_handle.IsValid()) {
            CHECK_ERRORS(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers_[draw.ib_handle.ID].id));
            CHECK_ERRORS(glDrawElements(
                GL_TRIANGLES,
                draw.ib_size ? draw.ib_size : index_buffers_[draw.ib_handle.ID].size,
                GL_UNSIGNED_INT,
                (void*)(size_t)(draw.ib_offset * sizeof(uint32_t))
            ));
        } else {
            CHECK_ERRORS(glDrawArrays(
                GL_TRIANGLES,
                0,
                draw.vb_size ? draw.vb_size : vb.size
            ));
        }

        glUseProgram(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

void GLContext::MakeCurrent() {
    static WindowHandle current_handle;

    if (handle_ == current_handle)
        return;

    glfwMakeContextCurrent(handle_);
    current_handle = handle_;
}

void GLContext::Init(GLContext &context) {
    context.MakeCurrent();
    int loaded = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    assert(loaded);
}

void GLContext::SwapBuffers() {
    glfwSwapBuffers(handle_);
}

GLContext GLContext::CreateDefault(const Config &config) {
    return GLContext {(WindowHandle) config.window_handle };
}

}

