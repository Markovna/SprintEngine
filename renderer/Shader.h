#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "color.h"
#include "GL.h"

#include <string>

namespace sprint {

class Shader {
private:
    explicit Shader(const std::string& source, std::initializer_list<gl::AttributeType::Enum> inTypes);

public:

    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;

    Shader(Shader&& other) noexcept ;
    Shader& operator=(Shader&& other);

    ~Shader();

    gl::ShaderHandle get_handle() const { return handle_; };

    static Shader Load(const std::string& path, std::initializer_list<gl::AttributeType::Enum> in_types);
private:
    gl::ShaderHandle handle_;

private:
    static void Swap(Shader& lhs, Shader& rhs);
};

}


