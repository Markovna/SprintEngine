#include "renderer_api_gl.h"
#include "Log.h"
#include "shader_utils.h"

namespace sprint {

namespace gfx {

namespace details {

static GLenum ToGLenum(uint32_t channels) {
    GLenum format = 0;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;
    return format;
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

static GLenum ToGLenum(ShaderType::Enum type) {
    switch (type) {
        case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
        case ShaderType::Vertex: return GL_VERTEX_SHADER;
    }
}

static uint32_t CompileShader(ShaderType::Enum type, const std::string& source) {
    uint32_t shaderID = glCreateShader(ToGLenum(type));
    const GLchar* sourceCStr = source.c_str();
    glShaderSource(shaderID, 1, &sourceCStr, NULL);
    glCompileShader(shaderID);
    CheckCompileStatus(shaderID);
    return shaderID;
}

GLRendererAPI::Shader::Shader(uint32_t id, const std::string &source, std::initializer_list<Attribute::Binding::Enum> bindings)
        : id_(id) {
    std::string shaders[ShaderType::Count];
    PreProcess(source, shaders);
    for (size_t i = 0; i < ShaderType::Count; i++) {
        auto id = CompileShader((ShaderType::Enum) i, shaders[i]);
        glAttachShader(id_, id);
        glDeleteShader(id);
    }

    glLinkProgram(id_);
    CheckLinkStatus(id_);

    uint32_t location = 0;
    for (auto binding : bindings) {
        attribute_locations_[binding] = location++;
        attributes_mask_ |= ((uint32_t) 1) << binding;
    }
}

bool GLRendererAPI::Shader::TryGetLocation(Attribute::Binding::Enum type, uint32_t &location) const {
    if (attributes_mask_ & (((uint32_t) 1) << type)) {
        location = attribute_locations_[type];
        return true;
    }
    return false;
}

void GLRendererAPI::CreateVertexBuffer(VertexBufferHandle handle, const void *data, uint32_t size, VertexLayout layout) {
    uint32_t bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, size * layout.get_stride(), data, GL_STATIC_DRAW); // TODO: static / dynamic ?
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    vertex_buffers_[handle.ID] = VertexBuffer(bufferId, size, layout);
}

void GLRendererAPI::CreateIndexBuffer(IndexBufferHandle handle, const void *data, uint32_t size) {
    uint32_t bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // TODO: static / dynamic ?
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    index_buffers_[handle.ID] = IndexBuffer(bufferId, size);
}

void GLRendererAPI::CreateShader(ShaderHandle handle,
                                 const std::string &source,
                                 std::initializer_list<Attribute::Binding::Enum> in_types) {
    shaders_[handle.ID] = Shader(glCreateProgram(), source, in_types);
}

void GLRendererAPI::CreateTexture(TextureHandle handle,
                                  const uint8_t *data,
                                  uint32_t width,
                                  uint32_t height,
                                  uint32_t channels) {
    uint32_t textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = ToGLenum(channels);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    textures_[handle.ID] = Texture(textureId);
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
}

void GLRendererAPI::Destroy(VertexBufferHandle handle) {
    glDeleteBuffers(1, &vertex_buffers_[handle.ID].id);
}

void GLRendererAPI::Destroy(IndexBufferHandle handle) {
    glDeleteBuffers(1, &index_buffers_[handle.ID].id);
}

void GLRendererAPI::Destroy(ShaderHandle handle) {
    glDeleteProgram(shaders_[handle.ID].get_id());
}

void GLRendererAPI::Destroy(TextureHandle handle) {
    glDeleteTextures(1, &textures_[handle.ID].id);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, TextureHandle texture_handle, int slot_idx) {
    glActiveTexture(GL_TEXTURE0 + slot_idx);
    glBindTexture(GL_TEXTURE_2D, textures_[handle.ID].id);
    SetUniform(handle, name, slot_idx);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, int value) {
    glUseProgram(shaders_[handle.ID].get_id());
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    glUniform1i(location, value);
    glUseProgram(0);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, bool value) {
    glUseProgram(shaders_[handle.ID].get_id());
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    glUniform1i(location, value);
    glUseProgram(0);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, float value) {
    glUseProgram(shaders_[handle.ID].get_id());
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    glUniform1f(location, value);
    glUseProgram(0);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, const Vec3& value) {
    glUseProgram(shaders_[handle.ID].get_id());
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
    glUseProgram(0);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, const Vec4& value) {
    glUseProgram(shaders_[handle.ID].get_id());
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
    glUseProgram(0);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, const Color& value) {
    glUseProgram(shaders_[handle.ID].get_id());
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    glUniform4f(location, value.r, value.g, value.b, value.a);
    glUseProgram(0);
}

void GLRendererAPI::SetUniform(ShaderHandle handle, const std::string& name, const Matrix& value) {
    glUseProgram(shaders_[handle.ID].get_id());
    uint32_t location = glGetUniformLocation(shaders_[handle.ID].get_id(), name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, (float*) &value);
    glUseProgram(0);
}

void GLContext::MakeCurrent() {
    glfwMakeContextCurrent(handle_);
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

}

}
