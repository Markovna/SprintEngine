#include "shader.h"
#include "log.h"

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
        log::core::Error("Couldn't read file {0}", path);
    }
    return content;
}

Shader::Shader(const std::string& source, std::initializer_list<gfx::Attribute::Binding::Enum> bindings)
    : handle_(gfx::CreateShader(source, bindings))
{
}

Shader::~Shader() {
    if (handle_)
        gfx::Destroy(handle_);
}

Shader Shader::Load(const std::string& path, std::initializer_list<gfx::Attribute::Binding::Enum> bindings) {
    return Shader(ReadFile(path), bindings);
}

Shader &Shader::operator=(Shader &&other) noexcept {
    if (handle_ != other.handle_) {
        Swap(*this, other);
    }
    return *this;
}

void Shader::Swap(Shader& lhs, Shader& rhs) {
    std::swap(lhs.handle_, rhs.handle_);
}

Shader::Shader(Shader &&other) noexcept {
    handle_ = other.handle_;
    other.handle_ = gfx::shader_handle::null;
}

}