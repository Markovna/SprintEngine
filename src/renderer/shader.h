#pragma once

#include "gfx.h"

namespace sprint {

class Shader {
private:
    explicit Shader(const std::string& source, std::initializer_list<gfx::Attribute::Binding::Enum> bindings);

public:

    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;

    Shader(Shader&& other) noexcept ;
    Shader& operator=(Shader&& other) noexcept;

    ~Shader();

    [[nodiscard]] gfx::shader_handle get_handle() const { return handle_; };

    static Shader Load(const std::string& path, std::initializer_list<gfx::Attribute::Binding::Enum> bindings);
private:
    gfx::shader_handle handle_;

private:
    static void Swap(Shader& lhs, Shader& rhs);
};

}


