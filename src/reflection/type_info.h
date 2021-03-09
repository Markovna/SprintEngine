#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "type_ids.h"
#include "field.h"
#include "method.h"

namespace meta::details {

class TypeInfo {
public:
    static const TypeInfo Invalid;

public:
    TypeInfo() noexcept
        : id_(InvalidTypeId), full_name_(), name_(), is_pointer_()
    {}

    TypeInfo(TypeId id, std::string full_name, std::string name, bool is_pointer)
        : id_(id), full_name_(std::move(full_name)), name_(std::move(name)), is_pointer_(is_pointer) {}

    [[nodiscard]] TypeId ID() const { return id_; }
    [[nodiscard]] const std::string &Name() const { return name_; }
    [[nodiscard]] const std::string &FullName() const { return full_name_; }
    [[nodiscard]] bool IsPointer() const { return is_pointer_; }
    [[nodiscard]] const std::vector<Field>& GetFields() const { return fields_; }
    [[nodiscard]] const std::vector<Method>& GetMethods() const { return methods_; }

    void AddField(Field field);
    [[nodiscard]] const Field& GetField(const std::string& name) const;
    [[nodiscard]] bool HasField(const std::string& name) const;

    void AddMethod(Method method);
    [[nodiscard]] const Method& GetMethod(const std::string& name) const;
    [[nodiscard]] bool HasMethod(const std::string& name) const;

private:
    const TypeId id_ = InvalidTypeId;
    const std::string full_name_;
    const std::string name_;
    const bool is_pointer_;
    std::vector<Field> fields_;
    std::unordered_map<std::string, uint32_t> name_to_field_idx_;
    std::vector<Method> methods_;
    std::unordered_multimap<std::string, uint32_t> name_to_method_idx_;
};

}