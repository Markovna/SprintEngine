#pragma once

#include "Vector.h"
#include <string>

enum ShaderType { Unknown, Vertex, Fragment };

class Shader {
public:
    explicit Shader(const std::string& source);

    void Use() const;
    void SetInt(const std::string& name, int value) const;
    void SetBool(const std::string& name, bool value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetFloat3(const std::string& name, Vec3 value) const;
    void SetFloat4(const std::string& name, Vec4 value) const;

    static std::shared_ptr<Shader> Load(const std::string& path);
private:
    unsigned int m_GLShaderID;
};


