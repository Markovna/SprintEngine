#include "type_info.h"

namespace meta::details {

const TypeInfo TypeInfo::Invalid = {};

void TypeInfo::AddField(Field field) {
    name_to_field_idx_[field.GetName()] = fields_.size();
    fields_.emplace_back(std::move(field));
}

const Field &TypeInfo::GetField(const std::string &name) const {
    if (auto it = name_to_field_idx_.find(name); it != name_to_field_idx_.end()) {
        return fields_[it->second];
    }

    return Field::Invalid();
}

bool TypeInfo::HasField(const std::string &name) const {
    return name_to_field_idx_.count(name);
}

void TypeInfo::AddMethod(Method method) {
    name_to_method_idx_.emplace(method.GetName(), methods_.size());
    methods_.emplace_back(std::move(method));
}

bool TypeInfo::HasMethod(const std::string& name) const {
    return name_to_method_idx_.count(name);
}

const Method& TypeInfo::GetMethod(const std::string& name) const {
    auto range = name_to_method_idx_.equal_range(name);
    if (range.first != range.second) {
        return methods_[range.first->second];
    }

    return Method::Invalid();
}

}