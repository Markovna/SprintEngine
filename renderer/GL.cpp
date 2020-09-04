#include "GL.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <unordered_map>

#include "IntAlloc.h"
#include "Log.h"

namespace Sprint {

namespace GL {

constexpr static uint16_t MAX_INDEX_BUFFERS_COUNT  = 1024;
constexpr static uint16_t MAX_VERTEX_BUFFERS_COUNT = 1024;
constexpr static uint16_t MAX_SHADERS_COUNT = 1024;

typedef uint32_t ATTRIB_LOCATION_MASK;

enum class ShaderType { Vertex, Fragment };

static GLenum ToGLenum(ShaderType type) {
    switch (type) {
        case ShaderType::Fragment: return GL_FRAGMENT_SHADER;
        case ShaderType::Vertex: return GL_VERTEX_SHADER;
    }
}

static bool TryParse(const std::string_view str, ShaderType& type) {
    if (str == "vertex") {
        type = ShaderType::Vertex;
        return true;
    }
    if (str == "fragment") {
        type = ShaderType::Fragment;
        return true;
    }
    return false;
}

static std::unordered_map<ShaderType, std::string> PreProcess(const std::string& source) {
    // TODO: optimize
    std::unordered_map<ShaderType, std::string> shaderSources;

    static const std::string typeToken = "#type";
    ShaderType type;
    size_t pos = source.find(typeToken, 0);
    while (pos != std::string::npos) {
        size_t eol = source.find_first_of('\n', pos);
        size_t begin = pos + typeToken.size() + 1;
        std::string typeStr = source.substr(begin, eol - begin);
        uint64_t nextLinePos = eol + 1;
        pos = source.find(typeToken, nextLinePos);

        if (TryParse(typeStr, type)) {
            shaderSources[type] = (pos == std::string::npos) ?
                                  source.substr(nextLinePos) :
                                  source.substr(nextLinePos, pos - nextLinePos);
        }
    }

    return shaderSources;
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

static uint32_t CompileShader(ShaderType type, const std::string& source) {
    uint32_t shaderID = glCreateShader(ToGLenum(type));
    const GLchar* sourceCStr = source.c_str();
    glShaderSource(shaderID, 1, &sourceCStr, NULL);
    glCompileShader(shaderID);
    CheckCompileStatus(shaderID);
    return shaderID;
}

class Shader {
public:
    Shader() noexcept : m_AttributesMask(0), m_ID(0) {}
    Shader(const std::string& source, std::initializer_list<AttributeType::Type> inTypes) : m_AttributesMask(0) {
        m_ID = glCreateProgram();
        for (const auto& keyValue : PreProcess(source)) {
            auto id = CompileShader(keyValue.first, keyValue.second);
            glAttachShader(m_ID, id);
            glDeleteShader(id);
        }

        glLinkProgram(m_ID);
        CheckLinkStatus(m_ID);

        uint32_t location = 0;
        for (auto type : inTypes) {
            m_AttributeLocations[type] = location++;
            m_AttributesMask |= ((uint32_t) 1) << type;
        }
    }

    void Bind() const {
        glUseProgram(m_ID);
    }

    void Unbind() const {
        glUseProgram(0);
    }

    void SetUniform(UniformType::Type type, const std::string &name, const void* value) const {
        glUseProgram(m_ID);
        uint32_t location = glGetUniformLocation(m_ID, name.c_str());
        float* ptr;
        switch (type) {
            case UniformType::Type::Int:
                glUniform1i(location, *(int*) value);
                break;
            case UniformType::Type::Float:
                glUniform1f(location, *(float*) value);
                break;
            case UniformType::Type::Vec3:
                ptr = (float*) value;
                glUniform3f(location, ptr[0], ptr[1], ptr[2]);
                break;
            case UniformType::Type::Vec4:
                ptr = (float*) value;
                glUniform4f(location, ptr[0], ptr[1], ptr[2], ptr[3]);
                break;
            case UniformType::Type::Mat4:
                glUniformMatrix4fv(location, 1, GL_FALSE, (float*) value);
                break;
        }
        glUseProgram(0);
    }

    void Destroy() {
        glDeleteProgram(m_ID);
        m_ID = 0;
        m_AttributesMask = 0;
    }

    bool TryGetLocation(AttributeType::Type type, uint32_t& location) const {
        if (m_AttributesMask & (((uint32_t) 1) << type)) {
            location = m_AttributeLocations[type];
            return true;
        }
        return false;
    }

private:
    uint32_t m_AttributesMask;
    uint32_t m_AttributeLocations[AttributeType::Count];
    uint32_t m_ID;
};

struct IndexBuffer {
    IndexBuffer() noexcept : ID(0), Size(0) {};
    explicit IndexBuffer(uint32_t id, uint32_t size) noexcept : ID(id), Size(size) {}

    uint32_t ID;
    uint32_t Size;
};

struct VertexBuffer {
    VertexBuffer() noexcept : ID(0), Size(0), Layout{} {};
    explicit VertexBuffer(uint32_t id, uint32_t size, VertexLayout layout) noexcept :
        ID(id), Size(size), Layout(layout)
    {}

    uint32_t ID;
    uint32_t Size;
    VertexLayout Layout;
};

typedef GLFWwindow* ContextWndHandle;

static VertexBufferHandle m_VertexBufferHandle { INVALID_HANDLE_ID };
static IndexBufferHandle m_IndexBufferHandle { INVALID_HANDLE_ID };
static uint32_t m_VertexArrayID = 0;
static ContextWndHandle m_CtxWndHandle;

static IndexBuffer m_IndexBuffers[MAX_INDEX_BUFFERS_COUNT];
static VertexBuffer m_VertexBuffers[MAX_VERTEX_BUFFERS_COUNT];
static Shader m_Shaders[MAX_SHADERS_COUNT];

static IntAlloc<MAX_INDEX_BUFFERS_COUNT, INVALID_HANDLE_ID>  m_IndexBufferIds;
static IntAlloc<MAX_VERTEX_BUFFERS_COUNT, INVALID_HANDLE_ID> m_VertexBufferIds;
static IntAlloc<MAX_SHADERS_COUNT, INVALID_HANDLE_ID> m_ShaderIds;

static GLbitfield ToGLBits(CLEAR_FLAG mask) {
    GLbitfield bitfield = 0;
    if (mask & CLEAR_DEPTH) bitfield |= (uint32_t) GL_DEPTH_BUFFER_BIT;
    if (mask & CLEAR_COLOR) bitfield |= (uint32_t) GL_COLOR_BUFFER_BIT;
    if (mask & CLEAR_STENCIL) bitfield |= (uint32_t) GL_STENCIL_BUFFER_BIT;
    return bitfield;
}

static GLenum ToGLenum(GLType type) {
    switch (type) {
        case GLType::FLOAT: return GL_FLOAT;
    }
}

static void CreateIndexBuffer(IndexBufferHandle handle, uint32_t *indices, uint32_t size) {
    uint32_t bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size * sizeof(uint32_t), indices, GL_STATIC_DRAW);

    m_IndexBuffers[handle.ID] = IndexBuffer(bufferId, size);
}

static void CreateVertexBuffer(VertexBufferHandle handle, float *data, uint32_t size, VertexLayout layout) {
    uint32_t bufferId;
    glGenBuffers(1, &bufferId);
    glBindBuffer(GL_ARRAY_BUFFER, bufferId);
    glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), data, GL_STATIC_DRAW);

    m_VertexBuffers[handle.ID] = VertexBuffer(bufferId, size, layout);
}

static void BindVertexBuffer(const VertexBuffer& buffer, uint32_t bindingIdx = 0) {
    glBindBuffer(GL_ARRAY_BUFFER, buffer.ID);
}

static void BindIndexBuffer(const IndexBuffer& buffer) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer.ID);
}

static void UnbindVertexBuffer(uint32_t bindingIdx = 0) {
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
            item.Attribute.Format.Size,
            ToGLenum(item.Attribute.Format.Type),
            item.Attribute.Normalized,
            stride,
            (void*)(item.Offset)
        );
}

static void BindAttributes(const Shader& shader, const VertexLayout& layout) {
    ATTRIB_LOCATION_MASK enabledAttribMask = 0;
    for (const auto& item : layout) {
        uint32_t idx;
        if (shader.TryGetLocation(item.Attribute.Type, idx)) {
            BindAttribute(idx, item, layout.GetStride());
            enabledAttribMask |= ((ATTRIB_LOCATION_MASK) 1) << idx;
        }
    }

    DisableAttributes(~enabledAttribMask);
}

void Clear(const Color &color, CLEAR_FLAG options) {
    glClearColor(color.R, color.G, color.B, color.A);
    glClear(ToGLBits(options));
}

void Init() {
    int loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    assert(loaded);

    glGenVertexArrays(1, &m_VertexArrayID);
    glBindVertexArray(m_VertexArrayID);

    glEnable(GL_DEPTH_TEST);

    m_CtxWndHandle = glfwGetCurrentContext();
}

void Shutdown() {
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &m_VertexArrayID);
}

IndexBufferHandle CreateIndexBuffer(uint32_t *indices, uint32_t size) {
    IndexBufferHandle handle(m_IndexBufferIds.Alloc());
    assert(IsValid(handle));
    CreateIndexBuffer(handle, indices, size);
    return handle;
}

void Destroy(IndexBufferHandle& handle) {
    glDeleteBuffers(1, &m_IndexBuffers[handle.ID].ID);
    m_IndexBufferIds.Free(handle.ID);
    handle.ID = INVALID_HANDLE_ID;
}

VertexBufferHandle CreateVertexBuffer(float *data, uint32_t size, VertexLayout layout) {
    VertexBufferHandle handle(m_VertexBufferIds.Alloc());
    assert(IsValid(handle));
    CreateVertexBuffer(handle, data, size, layout);
    return handle;
}

void Destroy(VertexBufferHandle &handle) {
    glDeleteBuffers(1, &m_VertexBuffers[handle.ID].ID);
    m_VertexBufferIds.Free(handle.ID);
    handle.ID = INVALID_HANDLE_ID;
}

void Bind(IndexBufferHandle handle) {
    m_IndexBufferHandle = handle;
    BindIndexBuffer(m_IndexBuffers[handle.ID]);
}

void Bind(VertexBufferHandle handle) {
    m_VertexBufferHandle = handle;
    BindVertexBuffer(m_VertexBuffers[handle.ID]);
}

void Render(ShaderHandle handle) {
    Shader& shader = m_Shaders[handle.ID];
    shader.Bind();
    if (IsValid(m_VertexBufferHandle)) {
        const VertexBuffer& vb = m_VertexBuffers[m_VertexBufferHandle.ID];
        BindAttributes(shader, vb.Layout);

        if (IsValid(m_IndexBufferHandle)) {
            const IndexBuffer& ib = m_IndexBuffers[m_IndexBufferHandle.ID];
            glDrawElements(GL_TRIANGLES, ib.Size, GL_UNSIGNED_INT, nullptr);
        }
        else {
            glDrawArrays(GL_TRIANGLES, 0, vb.Size);
        }
    }

    shader.Unbind();
    UnbindVertexBuffer();
    UnbindIndexBuffer();
}

ShaderHandle CreateShader(const std::string &source, std::initializer_list<AttributeType::Type> inTypes) {
    ShaderHandle handle(m_ShaderIds.Alloc());
    assert(IsValid(handle));
    m_Shaders[handle.ID] = Shader(source, inTypes);
    return handle;
}

void Destroy(ShaderHandle &handle) {
    m_Shaders[handle.ID].Destroy();
    m_ShaderIds.Free(handle.ID);
    handle.ID = INVALID_HANDLE_ID;
}

void SwapBuffers() {
    glfwSwapBuffers(m_CtxWndHandle);
}

void SetUniform(ShaderHandle handle, UniformType::Type type, const std::string &name, const void* value) {
    m_Shaders[handle.ID].SetUniform(type, name, value);
}

}

}