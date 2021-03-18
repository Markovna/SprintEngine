#pragma once

#include "reflection/meta_runtime.h"
#include "common/json.hpp"

namespace sprint::serializer {

void SerializeField(nlohmann::json& data, const meta::Field& field, meta::Reference reference);
void DeserializeField(nlohmann::json& data, const meta::Field& field, meta::Reference reference);

template<class T>
bool Deserialize(std::istream& in, T& object) {
    static const meta::Type type = meta::GetType<T>();
    static const std::vector<meta::Field>& fields = type.GetFields();

    meta::Reference ref(object);
    using json = nlohmann::json;
    json data;
    in >> data;
    if (data["type"] != type.FullName())
        return false;

    for (const auto& field : fields) {
        DeserializeField(data["data"], field, ref);
    }

    return true;
}

template<class T>
void Serialize(std::ostream& out, const T& object) {
    static const meta::Type type = meta::GetType<T>();
    static const std::vector<meta::Field>& fields = type.GetFields();

    meta::Reference ref(object);
    using json = nlohmann::json;
    json data;
    data["type"] = type.FullName();
    for (const auto& field : fields) {
        SerializeField(data["data"], field, ref);
    }

    out << std::setw(2) << data;
}


}