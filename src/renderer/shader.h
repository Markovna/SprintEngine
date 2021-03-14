#pragma once

#include "gfx.h"

namespace sprint {

class Shader {
public:
    explicit Shader(const std::string& source);
    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;

    Shader(Shader&& other) noexcept ;
    Shader& operator=(Shader&& other) noexcept;

    ~Shader();

    [[nodiscard]] gfx::shader_handle get_handle() const { return handle_; };

    static std::unique_ptr<Shader> Load(const std::istream& in);
private:
    gfx::shader_handle handle_;

private:
    static void Swap(Shader& lhs, Shader& rhs);
};

}


