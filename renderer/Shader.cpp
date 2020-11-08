#include "Shader.h"
#include "Log.h"

#include <fstream>
#include <sstream>

namespace sprint {

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

Shader::Shader(const std::string& source, std::initializer_list<gl::AttributeType::Enum> inTypes) :
    handle_(gl::CreateShader(source, std::move(inTypes)))
{
}

Shader::~Shader() {
    gl::Destroy(handle_);
}

void Shader::SetInt(const std::string& name, int value) const {
    gl::SetUniform(handle_, gl::UniformType::Int, name, &value);
}

void Shader::SetBool(const std::string& name, bool value) const {
    gl::SetUniform(handle_, gl::UniformType::Int, name, &value);
}

void Shader::SetFloat(const std::string& name, float value) const {
    gl::SetUniform(handle_, gl::UniformType::Float, name, &value);
}

void Shader::SetFloat3(const std::string& name, const Vec3& value) const {
    float values[] = {value.x, value.y, value.z };
    gl::SetUniform(handle_, gl::UniformType::Vec3, name, &values);
}

void Shader::SetFloat4(const std::string& name, const Vec4& value) const {
    float values[] = {value.x, value.y, value.z, value.w };
    gl::SetUniform(handle_, gl::UniformType::Vec4, name, &values);
}

void Shader::SetFloat4(const std::string &name, const Color &value) const {
    float values[] = {value.r, value.g, value.b, value.a };
    gl::SetUniform(handle_, gl::UniformType::Vec4, name, &values);
}

void Shader::SetMat(const std::string& name, const Matrix& value) const {
    gl::SetUniform(handle_, gl::UniformType::Mat4, name, &value[0][0]);
}

void Shader::Render() const {
    gl::Render(handle_);
}

std::shared_ptr<Shader> Shader::Load(const std::string& path, std::initializer_list<gl::AttributeType::Enum> in_types) {
    return std::shared_ptr<Shader>(new Shader(ReadFile(path), move(in_types)));
}

}