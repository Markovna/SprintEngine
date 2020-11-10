#pragma once

#include "Vector.h"
#include "Matrix.h"
#include "color.h"
#include "GL.h"

#include <string>

namespace sprint {

enum class ShaderType : uint8_t { Vertex, Fragment };

class Shader {
private:
    explicit Shader(const std::string& source, std::initializer_list<gl::AttributeType::Enum> inTypes);

public:

    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;

    ~Shader();

    void Render() const;

    void SetInt(const std::string& name, int value) const;
    void SetBool(const std::string& name, bool value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetFloat3(const std::string& name, const Vec3& value) const;
    void SetFloat4(const std::string& name, const Vec4& value) const;
    void SetFloat4(const std::string& name, const Color& value) const;
    void SetMat(const std::string& name, const Matrix& value) const;

    static std::shared_ptr<Shader> Load(const std::string& path, std::initializer_list<gl::AttributeType::Enum> in_types);
private:
    gl::ShaderHandle handle_;
};

}


