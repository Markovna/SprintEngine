#include "Shader.h"
#include "Log.h"

//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>

namespace Sprint {

static std::string ReadFile(const std::string& path) {
    std::string content;
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path, std::ios::in | std::ios::binary);
        std::stringstream sourceStream;
        sourceStream << file.rdbuf();
        file.close();
        content = sourceStream.str();
    }
    catch (std::ifstream::failure& e) {
        Log::CoreError("Couldn't read file {0}", path);
    }
    return content;
}

Shader::Shader(const std::string& source, std::initializer_list<GL::AttributeType::Type> inTypes) :
    m_Handle(GL::CreateShader(source, std::move(inTypes)))
{
}

Shader::~Shader() {
    GL::Destroy(m_Handle);
}

void Shader::SetInt(const std::string& name, int value) const {
    GL::SetUniform(m_Handle, GL::UniformType::Int, name, &value);
}

void Shader::SetBool(const std::string& name, bool value) const {
    GL::SetUniform(m_Handle, GL::UniformType::Int, name, &value);
}

void Shader::SetFloat(const std::string& name, float value) const {
    GL::SetUniform(m_Handle, GL::UniformType::Float, name, &value);
}

void Shader::SetFloat3(const std::string& name, const Vec3& value) const {
    float values[] = { value.X, value.Y, value.Z };
    GL::SetUniform(m_Handle, GL::UniformType::Vec3, name, &values);
}

void Shader::SetFloat4(const std::string& name, const Vec4& value) const {
    float values[] = { value.X, value.Y, value.Z, value.W };
    GL::SetUniform(m_Handle, GL::UniformType::Vec4, name, &values);
}

void Shader::SetFloat4(const std::string &name, const Color &value) const {
    float values[] = { value.R, value.G, value.B, value.A };
    GL::SetUniform(m_Handle, GL::UniformType::Vec4, name, &values);
}

void Shader::SetMat(const std::string& name, const Matrix& value) const {
    GL::SetUniform(m_Handle, GL::UniformType::Mat4, name, &value[0][0]);
}

void Shader::Render() const {
    GL::Render(m_Handle);
}

std::shared_ptr<Shader> Shader::Load(const std::string& path, std::initializer_list<GL::AttributeType::Type> inTypes) {
    return std::shared_ptr<Shader>(new Shader(ReadFile(path), move(inTypes)));
}

}