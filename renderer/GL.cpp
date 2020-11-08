#include "GL.h"

//#include <glad/glad.h>
//#include <GLFW/glfw3.h>

#include "IntAlloc.h"
#include "Log.h"

namespace sprint {

namespace gl {

typedef uint32_t ATTRIB_LOCATION_MASK;

static GLenum ToGLenum(ShaderType::Enum type) {
    switch (type) {
        case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
        case ShaderType::Vertex: return GL_VERTEX_SHADER;
    }
}

static void PreProcess(const std::string& source, std::string (&shaderSources)[ShaderType::Count]) {
    static const std::string typeToken = "#type";
    size_t pos = source.find(typeToken, 0);
    ShaderType::Enum type;
    while (pos != std::string::npos) {
        size_t eol = source.find_first_of('\n', pos);
        size_t begin = pos + typeToken.size() + 1;
        std::string typeStr = source.substr(begin, eol - begin);
        uint64_t nextLinePos = eol + 1;
        pos = source.find(typeToken, nextLinePos);

        if (ShaderType::TryParse(typeStr, type)) {
            shaderSources[type] = (pos == std::string::npos) ?
                                  source.substr(nextLinePos) :
                                  source.substr(nextLinePos, pos - nextLinePos);
        }
    }
}

static void CheckCompileStatus(uint32_t shaderID) {
    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        Log::CoreError("GL::Shader compilation failed with errors:\n{}", infoLog);
    }
}

static void CheckLinkStatus(uint32_t shaderID) {
    int success;
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
        Log::CoreError("GL::Shader linking failed with errors:\n{}", infoLog);
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

static GLbitfield ToGLBits(ClearFlag mask) {
    GLbitfield bitfield = 0;
    if (mask & kClearDepth) bitfield |= (uint32_t) GL_DEPTH_BUFFER_BIT;
    if (mask & kClearColor) bitfield |= (uint32_t) GL_COLOR_BUFFER_BIT;
    if (mask & kClearStencil) bitfield |= (uint32_t) GL_STENCIL_BUFFER_BIT;
    return bitfield;
}

static GLenum ToGLenum(GLType type) {
    switch (type) {
        case GLType::FLOAT: return GL_FLOAT;
    }
}

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

static void CreateTexture(TextureHandle handle, const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels) {
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

  textures[handle.ID] = Texture(textureId);
}

static void CreateIndexBuffer(IndexBufferHandle handle, uint32_t *indices, uint32_t size) {
    uint32_t bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint32_t), indices, GL_STATIC_DRAW);

  index_buffers[handle.ID] = IndexBuffer(bufferId, size);
}

static void CreateVertexBuffer(VertexBufferHandle handle, float *data, uint32_t size, VertexLayout layout) {
    uint32_t bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), data, GL_STATIC_DRAW);

  vertex_buffers[handle.ID] = VertexBuffer(bufferId, size, layout);
}

static void BindVertexBuffer(const VertexBuffer& buffer) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer.id);
}

static void BindIndexBuffer(const IndexBuffer& buffer) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.id);
}

static void UnbindVertexBuffer() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

static void UnbindIndexBuffer() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static void DisableAttributes(ATTRIB_LOCATION_MASK mask) {
    while (mask) {
        uint32_t index = __builtin_ctzl(mask);
        glDisableVertexAttribArray(index);
        mask ^= ((uint64_t)1) << index;
    }
}

static void BindAttribute(uint32_t attributeIdx, const VertexLayout::Item& item, uint32_t stride) {
    glEnableVertexAttribArray(attributeIdx);
    glVertexAttribPointer(
            attributeIdx,
            item.attribute.format.size,
            ToGLenum(item.attribute.format.type),
            item.attribute.normalized,
            stride,
            (void*)(item.offset)
        );
}

static void BindAttributes(const Shader& shader, const VertexLayout& layout) {
    ATTRIB_LOCATION_MASK enabledAttribMask = 0;
    for (const auto& item : layout) {
        uint32_t idx;
        if (shader.TryGetLocation(item.attribute.type, idx)) {
            BindAttribute(idx, item, layout.get_stride());
            enabledAttribMask |= ((ATTRIB_LOCATION_MASK) 1) << idx;
        }
    }

    DisableAttributes(~enabledAttribMask);
}

void Clear(const Color &color, ClearFlag options) {
    glClearColor(color.r, color.g, color.b, color.a);
    glClear(ToGLBits(options));
}

void Init() {
    int loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    assert(loaded);

    glGenVertexArrays(1, &vertex_array_id);
    glBindVertexArray(vertex_array_id);

    glEnable(GL_DEPTH_TEST);

  ctx_window_handle = glfwGetCurrentContext();
}

void Shutdown() {
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &vertex_array_id);
}

IndexBufferHandle CreateIndexBuffer(uint32_t *indices, uint32_t size) {
    IndexBufferHandle handle(index_buffer_ids.Alloc());
    assert(handle.IsValid());
    CreateIndexBuffer(handle, indices, size);
    return handle;
}

void Destroy(IndexBufferHandle& handle) {
    glDeleteBuffers(1, &index_buffers[handle.ID].id);
    index_buffer_ids.Free(handle.ID);
    handle = IndexBufferHandle::Invalid;
}

VertexBufferHandle CreateVertexBuffer(float *data, uint32_t size, VertexLayout layout) {
    VertexBufferHandle handle(vertex_buffer_ids.Alloc());
    assert(handle.IsValid());
    CreateVertexBuffer(handle, data, size, layout);
    return handle;
}

void Destroy(VertexBufferHandle &handle) {
    glDeleteBuffers(1, &vertex_buffers[handle.ID].id);
    vertex_buffer_ids.Free(handle.ID);
    handle = VertexBufferHandle::Invalid;
}

void Bind(IndexBufferHandle handle) {
  index_buffer_handle = handle;
    BindIndexBuffer(index_buffers[handle.ID]);
}

void Bind(VertexBufferHandle handle) {
  vertex_buffer_handle = handle;
    BindVertexBuffer(vertex_buffers[handle.ID]);
}

void Render(ShaderHandle handle) {
    Shader& shader = shaders[handle.ID];
    shader.Use();
    if (vertex_buffer_handle.IsValid()) {
        const VertexBuffer& vb = vertex_buffers[vertex_buffer_handle.ID];
        BindAttributes(shader, vb.layout);

        if (index_buffer_handle.IsValid()) {
            const IndexBuffer& ib = index_buffers[index_buffer_handle.ID];
            glDrawElements(GL_TRIANGLES, ib.size, GL_UNSIGNED_INT, nullptr);
        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, vb.size);
        }
    }

    glUseProgram(0);

    UnbindVertexBuffer();
    UnbindIndexBuffer();

  vertex_buffer_handle = VertexBufferHandle::Invalid;
  index_buffer_handle = IndexBufferHandle::Invalid;
}

ShaderHandle CreateShader(const std::string &source, std::initializer_list<AttributeType::Enum> in_types) {
    ShaderHandle handle(shader_ids.Alloc());
    assert(handle.IsValid());
  shaders[handle.ID] = Shader(source, in_types);
    return handle;
}

TextureHandle CreateTexture(const uint8_t *data, uint32_t width, uint32_t height, uint32_t channels) {
    TextureHandle handle(texture_ids.Alloc());
    assert(handle.IsValid());
    CreateTexture(handle, data, width, height, channels);
    return handle;
}

void Destroy(ShaderHandle &handle) {
    shaders[handle.ID].Destroy();
    shader_ids.Free(handle.ID);
    handle = ShaderHandle::Invalid;
}

void SwapBuffers() {
    glfwSwapBuffers(ctx_window_handle);
}

void Destroy(TextureHandle &handle) {
    glDeleteTextures(1, &textures[handle.ID].id);
    texture_ids.Free(handle.ID);
    handle = TextureHandle::Invalid;
}

void Bind(TextureHandle handle, uint32_t idx) {
    glActiveTexture(GL_TEXTURE0 + idx);
    glBindTexture(GL_TEXTURE_2D, textures[handle.ID].id);
}

void SetUniform(ShaderHandle handle, UniformType::Enum type, const std::string &name, const void *values) {
    shaders[handle.ID].SetUniform(type, name, values);
}

Shader::Shader(const std::string &source, std::initializer_list<AttributeType::Enum> inTypes) : attributes_mask_(0) {
  id_ = glCreateProgram();
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
    for (auto type : inTypes) {
      attribute_locations_[type] = location++;
        attributes_mask_ |= ((uint32_t) 1) << type;
    }
}

void Shader::Use() const {
    glUseProgram(id_);
}

void Shader::SetUniform(UniformType::Enum type, const std::string &name, const void *value) const {
    glUseProgram(id_);
    uint32_t location = glGetUniformLocation(id_, name.c_str());
    float* ptr;
    switch (type) {
        case UniformType::Enum::Int:
            glUniform1i(location, *(int*) value);
            break;
        case UniformType::Enum::Float:
            glUniform1f(location, *(float*) value);
            break;
        case UniformType::Enum::Vec3:
            ptr = (float*) value;
            glUniform3f(location, ptr[0], ptr[1], ptr[2]);
            break;
        case UniformType::Enum::Vec4:
            ptr = (float*) value;
            glUniform4f(location, ptr[0], ptr[1], ptr[2], ptr[3]);
            break;
        case UniformType::Enum::Mat4:
            glUniformMatrix4fv(location, 1, GL_FALSE, (float*) value);
            break;
    }
    glUseProgram(0);
}

void Shader::Destroy() {
    glDeleteProgram(id_);
  id_ = 0;
    attributes_mask_ = 0;
}

bool Shader::TryGetLocation(AttributeType::Enum type, uint32_t &location) const {
    if (attributes_mask_ & (((uint32_t) 1) << type)) {
        location = attribute_locations_[type];
        return true;
    }
    return false;
}

std::string UniformType::ToString(UniformType::Enum type) {
    switch (type) {
        case Int: return "Int";
        case Float: return "Float";
        case Vec3: return "Vec3";
        case Vec4: return "Vec4";
        case Mat4: return "Mat4";
    }
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

}

}