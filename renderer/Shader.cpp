#include "Shader.h"
#include "Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>

static std::string ReadFile(const std::string& path) {
    std::string content;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        file.open(path);
        std::stringstream sourceStream;
        sourceStream << file.rdbuf();
        file.close();
        content = sourceStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        Log::CoreError("Couldn't read file {0}", path);
    }
    return content;
}

static unsigned int CompileShader(ShaderType type, const std::string& source) {
    unsigned int shaderID = glCreateShader(
                type == ShaderType::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER);
    const GLchar* sourceCStr = source.c_str();
    glShaderSource(shaderID, 1, &sourceCStr, NULL);
    glCompileShader(shaderID);

    int success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shaderID, 512, NULL, infoLog);
        Log::CoreError("Shader compilation failed with errors:\n{0}", infoLog);
    }
    return shaderID;
}

static ShaderType StringToShaderType(const std::string& str) {
    if (str == "vertex")   return ShaderType::Vertex;
    if (str == "fragment") return ShaderType::Fragment;
    return ShaderType::Unknown;
}

static std::unordered_map<ShaderType, std::string> PreProcess(const std::string& source) {
    std::unordered_map<ShaderType, std::string> shaderSources;

    const std::string typeToken = "#type";
    size_t pos = source.find(typeToken, 0);
    while (pos != std::string::npos)
    {
        size_t eol = source.find_first_of('\n', pos);
        size_t begin = pos + typeToken.size() + 1;
        std::string typeStr = source.substr(begin, eol - begin);
        unsigned long nextLinePos = eol + 1;
        pos = source.find(typeToken, nextLinePos);
        shaderSources[StringToShaderType(typeStr)] = (pos == std::string::npos) ?
                source.substr(nextLinePos) :
                source.substr(nextLinePos, pos - nextLinePos);
    }

    return shaderSources;
}

Shader::Shader(const std::string& source) {
    m_GLShaderID = glCreateProgram();
    for (const auto& [type, source] : PreProcess(source)) {
        auto shaderID = CompileShader(type, source);
        glAttachShader(m_GLShaderID, shaderID);
        glDeleteShader(shaderID);
    }

    glLinkProgram(m_GLShaderID);
    int success;
    glGetProgramiv(m_GLShaderID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_GLShaderID, 512, NULL, infoLog);
        Log::CoreError("Shader linking failed with errors:\n{0}", infoLog);
    }
}

void Shader::SetInt(const std::string& name, int value) const {
    glUniform1i(glGetUniformLocation(m_GLShaderID, name.c_str()), value);
}

void Shader::SetBool(const std::string& name, bool value) const {
    glUniform1i(glGetUniformLocation(m_GLShaderID, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const {
    glUniform1f(glGetUniformLocation(m_GLShaderID, name.c_str()), value);
}

void Shader::SetFloat3(const std::string& name, Vec3 value) const {
    glUniform3f(glGetUniformLocation(m_GLShaderID, name.c_str()), value.X, value.Y, value.Z);
}

void Shader::SetFloat4(const std::string& name, Vec4 value) const {
    glUniform4f(glGetUniformLocation(m_GLShaderID, name.c_str()), value.X, value.Y, value.Z, value.W);
}

void Shader::Use() const {
    glUseProgram(m_GLShaderID);
}

std::shared_ptr<Shader> Shader::Load(const std::string& path) {
    return std::make_shared<Shader>(ReadFile(path));
}