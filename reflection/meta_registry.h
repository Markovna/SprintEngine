#pragma once

#include <unordered_map>
#include <vector>

#include "type_info.h"

namespace meta::details {

static std::string GetName(const std::string& full_name) {
    auto pos = full_name.find_last_of("::");
    if (pos == std::string::npos) {
        return full_name;
    }

    return full_name.substr(pos + 1);
}

class MetaRegistry {
public:
    template<class T>
    TypeInfo& AddType(const std::string &full_name) {
        using type = std::decay_t<T>;
        bool is_pointer = std::is_pointer_v<T>;

        TypeId id = TypeIds<type>::id = types_.size();
        TypeInfo info(id, full_name, GetName(full_name), is_pointer);
        if (!info.IsPointer())
            name_to_ids_[full_name] = info.ID();

        return types_.emplace_back(std::move(info));
    }

    TypeInfo &operator[](TypeId id) {
        return types_[id];
    }

    const TypeInfo &operator[](TypeId id) const {
        return types_[id];
    }

    const TypeInfo &operator[](const std::string &name) const {
        return types_[name_to_ids_.at(name)];
    }

    [[nodiscard]] const TypeInfo& Get(TypeId id) const {
        if (id < types_.size())
            return types_[id];

        return TypeInfo::Invalid;
    }

    [[nodiscard]] const TypeInfo &Get(const std::string &name) const {
        if (auto it = name_to_ids_.find(name); it != name_to_ids_.end()) {
            return types_[it->second];
        }
        return TypeInfo::Invalid;
    }

private:
    std::vector<TypeInfo> types_;
    std::unordered_map<std::string, TypeId> name_to_ids_;
};

inline MetaRegistry& Registry() {
    static MetaRegistry instance;
    return instance;
}

const TypeInfo& GetTypeInfo(TypeId id);
const TypeInfo& GetTypeInfo(std::string name);

}

