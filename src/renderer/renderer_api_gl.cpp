#include "renderer_api_gl.h"
#include "core/log.h"
#include "shader_utils.h"

namespace sprint::gfx::details {

#define RENDERER_API_PROFILE 1
#define RENDERER_API_DEBUG 1

#if RENDERER_API_PROFILE
#   define SPRINT_RENDERER_PROFILE_SCOPE(__name) SPRINT_PROFILE_SCOPE(__name)
#   define SPRINT_RENDERER_PROFILE_FUNCTION() SPRINT_PROFILE_FUNCTION()
#else
#   define SPRINT_RENDERER_PROFILE_SCOPE(__name)
#   define SPRINT_RENDERER_PROFILE_FUNCTION()
#endif

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
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        log::core::Error("GL::Shader compilation failed with errors:\n{}", infoLog);
    }
}

static void CheckLinkStatus(uint32_t shaderID) {
    int success;
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderID, 512, nullptr, infoLog);
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
    CHECK_ERRORS(glShaderSource(shaderID, 1, &sourceCStr, nullptr));
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

bool GLRendererAPI::Shader::TryGetLocation(Attribute::Binding::Enum type, uint16_t& location) const {
    if (attributes_mask & (1u << type)) {
        location = attribute_locations[type];
        return true;
    }
    return false;
}

void GLRendererAPI::CreateVertexBuffer(vertexbuf_handle handle, const void* data, uint32_t data_size, uint32_t size, VertexLayout layout) {
    uint32_t bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    CHECK_ERRORS(glBufferData(GL_ARRAY_BUFFER, data_size, data, data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    VertexBuffer& vertex_buffer = vertex_buffers_[handle.index()];
    vertex_buffer.id = bufferId;
    vertex_buffer.size = size;
    vertex_buffer.layout = layout;
}

void GLRendererAPI::CreateIndexBuffer(indexbuf_handle handle, const void* data, uint32_t data_size, uint32_t size) {
    uint32_t bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
    CHECK_ERRORS(glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, data, data ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    IndexBuffer& index_buffer = index_buffers_[handle.index()];
    index_buffer.id = bufferId;
    index_buffer.size = size;
}

void GLRendererAPI::CreateFrameBuffer(framebuf_handle handle, texture_handle* handles, uint32_t num, bool destroy_tex) {
    uint32_t bufferId;
    glGenFramebuffers(1, &bufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, bufferId);

    FrameBuffer& frame_buffer = frame_buffers_[handle.index()];

    for (int i = 0; i < num; i++) {
        texture_handle tex = handles[i];
        Texture& texture = textures_[tex.index()];
        uint32_t tex_id = texture.id;
        TextureFormat::Enum format = texture.format;
        GLenum attachment;

        auto& format_info = TextureFormat::GetInfo(format);
        bool is_depth = format_info.depth_bits || format_info.stencil_bits;
        if (is_depth) {
            if (format_info.stencil_bits && format_info.depth_bits) {
                attachment = GL_DEPTH_STENCIL_ATTACHMENT;
            } else if (format_info.depth_bits) {
                attachment = GL_DEPTH_ATTACHMENT;
            } else {
                attachment = GL_STENCIL_ATTACHMENT;
            }
        } else {
            attachment = GL_COLOR_ATTACHMENT0;
        }

        if (texture.render_buffer) {
            CHECK_ERRORS(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, tex_id));
        } else {
            CHECK_ERRORS(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, tex_id, 0));
        }

        frame_buffer.tex_handles[i] = tex;
    }

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    frame_buffer.id = bufferId;
    frame_buffer.tex_num = num;
    frame_buffer.destroy_tex = destroy_tex;
}

void GLRendererAPI::UpdateVertexBuffer(vertexbuf_handle handle, uint32_t offset, const void* data, uint32_t data_size) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers_[handle.index()].id);
    CHECK_ERRORS(glBufferSubData(GL_ARRAY_BUFFER, offset, data_size, data));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GLRendererAPI::UpdateIndexBuffer(indexbuf_handle handle, uint32_t offset, const void* data, uint32_t data_size) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffers_[handle.index()].id);
    CHECK_ERRORS(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, data_size, data));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GLRendererAPI::CreateShader(shader_handle handle, const std::string &source) {
    std::string vertex_shader, fragment_shader;
    Attribute::BindingPack binding;
    PreProcess(source, vertex_shader, fragment_shader, binding);

    uint32_t id = glCreateProgram();

    auto vertex_shader_id = CompileShader(ShaderType::Vertex, vertex_shader);
    glAttachShader(id, vertex_shader_id);
    glDeleteShader(vertex_shader_id);


    auto fragment_shader_id = CompileShader(ShaderType::Fragment, fragment_shader);
    glAttachShader(id, fragment_shader_id);
    glDeleteShader(fragment_shader_id);

    CHECK_ERRORS(glLinkProgram(id));
    CheckLinkStatus(id);

    Shader& shader = shaders_[handle.index()];
    shader.id = id;
    shader.attributes_mask = binding.mask;
    std::memcpy(shader.attribute_locations, binding.locations, Attribute::Binding::Count);
    shader.model_location = glGetUniformLocation(id, "model");
    shader.view_location = glGetUniformLocation(id, "view");
    shader.proj_location = glGetUniformLocation(id, "projection");
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

void GLRendererAPI::CreateUniform(uniform_handle handle, char* name) {
    UniformInfo& uniform = uniforms_[handle.index()];
    std::strcpy(uniform.name, name);
}

void GLRendererAPI::CreateTexture(texture_handle handle,
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

    Texture& texture = textures_[handle.index()];
    texture.id = textureId;
    texture.render_buffer = render_buffer;
    texture.format = format;
    texture.width = width;
    texture.height = height;
}

GLRendererAPI::~GLRendererAPI() {
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vao_);
}

static void SetDepthTest(bool enable) {
    static bool enabled = false;
    if (enabled == enable) return;

    enabled = enable;
    if (enable) {
        CHECK_ERRORS(glEnable(GL_DEPTH_TEST));
    } else {
        CHECK_ERRORS(glDisable(GL_DEPTH_TEST));
    }
}

GLRendererAPI::GLRendererAPI(const Config &config)
        : default_context_(GLContext::CreateDefault(config)) {

    GLContext::Init(default_context_);

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    SetDepthTest(true);

    /**/

    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
//    glDisable(GL_DEPTH_TEST);
//    glEnable(GL_SCISSOR_TEST);

     /**/
}

void GLRendererAPI::Destroy(vertexbuf_handle handle) {
    glDeleteBuffers(1, &vertex_buffers_[handle.index()].id);
}

void GLRendererAPI::Destroy(indexbuf_handle handle) {
    glDeleteBuffers(1, &index_buffers_[handle.index()].id);
}

void GLRendererAPI::Destroy(framebuf_handle handle) {
    FrameBuffer& frame_buffer = frame_buffers_[handle.index()];
    if (frame_buffer.destroy_tex) {
        for (int i = 0; i < frame_buffer.tex_num; i++) {
            Destroy(frame_buffer.tex_handles[i]);
        }
    }

    glDeleteFramebuffers(1, &frame_buffer.id);
}

void GLRendererAPI::Destroy(shader_handle handle) {
    glDeleteProgram(shaders_[handle.index()].id);
}

void GLRendererAPI::Destroy(texture_handle handle) {
    if (textures_[handle.index()].render_buffer) {
        glDeleteRenderbuffers(1, &textures_[handle.index()].id);
    } else {
        glDeleteTextures(1, &textures_[handle.index()].id);
    }
}

static inline GLenum ToType(Attribute::Type::Enum type) {
    static GLenum types[] = {
        GL_FLOAT, GL_INT, GL_UNSIGNED_INT, GL_UNSIGNED_BYTE
    };
    return types[type];
}

static GLbitfield ToGLBits(ClearFlag::Type mask) {
    GLbitfield bitfield = 0;
    if (mask & ClearFlag::Depth) bitfield |= (uint32_t) GL_DEPTH_BUFFER_BIT;
    if (mask & ClearFlag::Color) bitfield |= (uint32_t) GL_COLOR_BUFFER_BIT;
    if (mask & ClearFlag::Stencil) bitfield |= (uint32_t) GL_STENCIL_BUFFER_BIT;
    return bitfield;
}

static void DisableAttributes(uint16_t mask) {
    while (mask) {
        uint16_t index = __builtin_ctzl(mask);
        CHECK_ERRORS(glDisableVertexAttribArray(index));
        mask ^= 1u << index;
    }
}

static void SetViewport(RectInt rect, Vec2Int resolution) {
    CHECK_ERRORS(glViewport(
        rect.x,
        resolution.y - (rect.y + rect.height),
        rect.width,
        rect.height
    ));
}

static void SetScissor(RectInt scissor, Vec2Int resolution) {
    if (Valid(scissor)) {
        CHECK_ERRORS(glEnable(GL_SCISSOR_TEST));
        CHECK_ERRORS(glScissor(
            scissor.x,
            resolution.y - (scissor.y + scissor.height),
            scissor.width,
            scissor.height
        ));
    }
    else {
        CHECK_ERRORS(glDisable(GL_SCISSOR_TEST));
    }
}

static void SetUniformValue(int loc, const Uniform& value) {
    switch (value.type) {
        case Uniform::INT: { CHECK_ERRORS(glUniform1i(loc, (int)value)); break; }
        case Uniform::BOOL: { CHECK_ERRORS(glUniform1i(loc, (bool)value)); break; }
        case Uniform::FLOAT: { CHECK_ERRORS(glUniform1f(loc, (float)value)); break; }
        case Uniform::VEC3: {
            auto& vec = std::get<Vec3>(value);
            CHECK_ERRORS(glUniform3f(loc, vec.x, vec.y, vec.z));
            break;
        }
        case Uniform::VEC4: {
            auto& vec = std::get<Vec4>(value);
            CHECK_ERRORS(glUniform4f(loc, vec.x, vec.y, vec.z, vec.w));
            break;
        }
        case Uniform::COLOR: {
            auto& vec = std::get<Color>(value);
            CHECK_ERRORS(glUniform4f(loc, vec.r, vec.g, vec.b, vec.a));
            break;
        }
        case Uniform::MATRIX: {
            auto& vec = std::get<Matrix>(value);
            CHECK_ERRORS(glUniformMatrix4fv(loc, 1, GL_FALSE, (float*) &vec));
            break;
        }
    }
}

void GLRendererAPI::RenderFrame(const Frame& frame) {

    SPRINT_PROFILE_FUNCTION();

    default_context_.MakeCurrent();

    CHECK_ERRORS(glDisable(GL_SCISSOR_TEST));
    for (auto& camera : frame.get_cameras()) {
        if (camera.frame_buffer) {
            CHECK_ERRORS(glBindFramebuffer(GL_FRAMEBUFFER, frame_buffers_[camera.frame_buffer.index()].id));
        }

        CHECK_ERRORS(glClearColor(camera.clear_color.r, camera.clear_color.g, camera.clear_color.b, camera.clear_color.a));
        CHECK_ERRORS(glClear(ToGLBits(camera.clear_flag)));

        if (camera.frame_buffer) {
            CHECK_ERRORS(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }
    }

    static Vec2Int resolution;
    Shader* shader = nullptr;
    CameraId camera_id = UINT16_MAX;
    RectInt scissor;
    uint32_t vb_id = 0;
    uint32_t fb_id = 0;
    uint32_t vb_offset = 0;

    bool resolution_changed = resolution != frame.get_resolution();
    resolution = frame.get_resolution();

    for (const DrawUnit& draw : frame.get_draws()) {
        SPRINT_RENDERER_PROFILE_SCOPE("GLRendererAPI::RenderFrame [render a draw call]");

        const Camera& camera = frame.get_camera(draw.camera_id);

        uint32_t curr_shader_id = shaders_[draw.shader_handle.index()].id;
        uint32_t curr_vb_id = draw.vb_handle ? vertex_buffers_[draw.vb_handle.index()].id : 0;
        uint32_t curr_fb_id = camera.frame_buffer ? frame_buffers_[camera.frame_buffer.index()].id : 0;

        bool shader_changed = shader == nullptr || shader->id != curr_shader_id;
        bool camera_changed = camera_id != draw.camera_id;
        bool scissor_changed = scissor != draw.scissor;
        bool vb_changed = curr_vb_id != vb_id;
        bool fb_changed = curr_fb_id != fb_id;
        bool vb_offset_changed = vb_offset != draw.vb_offset;

        shader = draw.shader_handle ? &shaders_[draw.shader_handle.index()] : nullptr;
        camera_id = draw.camera_id;
        scissor = draw.scissor;
        fb_id = curr_fb_id;
        vb_id = curr_vb_id;
        vb_offset = draw.vb_offset;

        if (fb_changed) {
            CHECK_ERRORS(glBindFramebuffer(GL_FRAMEBUFFER, fb_id));

            if (camera.frame_buffer) {
                texture_handle tex = frame_buffers_[camera.frame_buffer.index()].tex_handles[0];
                Vec2Int tex_resolution = {(int) textures_[tex.index()].width, (int)textures_[tex.index()].height};
                resolution_changed |= resolution != tex_resolution;
                resolution = tex_resolution;
            } else {
                resolution_changed |= resolution != frame.get_resolution();
                resolution = frame.get_resolution();
            }
        }

        if (shader_changed) {
            CHECK_ERRORS(glUseProgram(shader->id));
        }

        {
            SPRINT_RENDERER_PROFILE_SCOPE("GLRendererAPI::RenderFrame [set matrix uniforms]");

            CHECK_ERRORS(glUniformMatrix4fv(shader->model_location, 1, GL_FALSE, (float *) &draw.transform));
            CHECK_ERRORS(glUniformMatrix4fv(shader->view_location, 1, GL_FALSE, (float *) &camera.view));
            CHECK_ERRORS(glUniformMatrix4fv(shader->proj_location, 1, GL_FALSE, (float *) &camera.projection));
        }

        {
            SPRINT_RENDERER_PROFILE_SCOPE("GLRendererAPI::RenderFrame [set shader uniforms]");

            for (int i = 0; i < draw.uniforms_size; i++) {
                const UniformPair& pair = draw.uniforms[i];
                UniformInfo& uniform = uniforms_[pair.handle.index()];
                int loc = glGetUniformLocation(shader->id, uniform.name);
                SetUniformValue(loc, pair.value);
            }

            for (int i = 0; i < draw.texture_slots_size; i++) {
                uint32_t slot_idx = draw.texture_slots[i];
                texture_handle tex_handle = draw.textures[slot_idx];
                CHECK_ERRORS(glActiveTexture(GL_TEXTURE0 + slot_idx));
                CHECK_ERRORS(glBindTexture(GL_TEXTURE_2D, textures_[tex_handle.index()].id));
            }
        }

        if (camera_changed || resolution_changed) {
            SetViewport(camera.viewport, resolution);
        }

        if (scissor_changed || resolution_changed) {
            SetScissor(draw.scissor, resolution);
        }

        SetDepthTest(draw.options & DrawConfig::DEPTH_TEST);

        if (vb_changed) {
            CHECK_ERRORS(glBindBuffer(GL_ARRAY_BUFFER, vb_id));
        }

        if (draw.vb_handle) {
            SPRINT_RENDERER_PROFILE_SCOPE("GLRendererAPI::RenderFrame [set vertex attributes array]");

            VertexLayout& layout = vertex_buffers_[draw.vb_handle.index()].layout;

            uint16_t enabledAttribMask = 0;
            uint32_t base_vertex = draw.vb_offset * layout.get_stride();
            {
                SPRINT_RENDERER_PROFILE_SCOPE("GLRendererAPI::RenderFrame [enable attributes]");

                for (const auto &item : layout) {
                    uint16_t idx;
                    if (shader->TryGetLocation(item.attribute.binding, idx)) {
                        CHECK_ERRORS(glVertexAttribPointer(
                            idx,
                            item.attribute.format.size,
                            ToType(item.attribute.format.type),
                            item.attribute.normalized,
                            layout.get_stride(),
                            (void*) (base_vertex + item.offset)
                        ));
                        uint16_t mask = 1u << idx;
                        enabledAttribMask |= mask;
                        if (!(shader->enabled_attributes_mask & mask)) {
                            CHECK_ERRORS(glEnableVertexAttribArray(idx));
                        }
                    }
                }
            }

            uint16_t disable = uint16_t(enabledAttribMask ^ shader->enabled_attributes_mask) & shader->enabled_attributes_mask;
            shader->enabled_attributes_mask = enabledAttribMask;
            DisableAttributes(disable);
        }

        if (draw.vb_handle) {
            SPRINT_RENDERER_PROFILE_SCOPE("GLRendererAPI::RenderFrame [draw vertex array]");

            if (draw.ib_handle) {
                IndexBuffer& index_buffer = index_buffers_[draw.ib_handle.index()];
                CHECK_ERRORS(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer.id));
                CHECK_ERRORS(glDrawElements(
                    GL_TRIANGLES,
                    draw.ib_size ? draw.ib_size : index_buffer.size,
                    GL_UNSIGNED_INT,
                    (void *) (size_t) (draw.ib_offset * sizeof(uint32_t))
                ));
            } else {
                CHECK_ERRORS(glDrawArrays(
                    GL_TRIANGLES,
                    0,
                    draw.vb_size ? draw.vb_size : vertex_buffers_[draw.vb_handle.index()].size
                ));
            }
        }
    }

    glUseProgram(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    default_context_.SwapBuffers();
}

void GLRendererAPI::Destroy(uniform_handle) {
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
    SPRINT_RENDERER_PROFILE_FUNCTION();
    glfwSwapBuffers(handle_);
}

GLContext GLContext::CreateDefault(const Config &config) {
    return GLContext {(WindowHandle) config.window_handle };
}

}

