#include "shader.h"
#include "core/log.h"

#include <fstream>

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

Shader::Shader(const std::string& source)
    : handle_(gfx::CreateShader(source))
{
}

Shader::~Shader() {
    if (handle_)
        gfx::Destroy(handle_);
}

std::unique_ptr<Shader> Shader::Load(const std::istream& file, const std::istream& meta) {
    std::stringstream ss;
    ss << file.rdbuf();
    std::string source(ss.str());
    return std::make_unique<Shader>(source);
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