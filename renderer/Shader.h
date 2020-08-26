#pragma once

#include "Vector.h"
#include "Matrix.h"
#include <string>
#include <unordered_map>

enum ShaderType { Unknown, Vertex, Fragment };

class Shader {
private:
    explicit Shader(const std::string& source);

public:

    Shader(const Shader& other) = delete;
    Shader& operator=(const Shader& other) = delete;

    ~Shader();

    void Bind() const;

    void SetInt(const std::string& name, int value) const;
    void SetBool(const std::string& name, bool value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetFloat3(const std::string& name, const Vec3& value) const;
    void SetFloat4(const std::string& name, const Vec4& value) const;
    void SetMat(const std::string& name, const Matrix& value) const;

    bool TryGetAttributeID(const std::string& name, unsigned int& id) const;

    static std::shared_ptr<Shader> Load(const std::string& path);

private:
    void PopulateAttributes();

private:
    unsigned int m_GLShaderID;
    std::unordered_map<std::string, unsigned int> m_Attributes;
};


