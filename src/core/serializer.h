#pragma once

#include "reflection/meta_runtime.h"
#include "common/json.hpp"

namespace sprint::serializer {

void DeserializeField(nlohmann::json& data, const meta::Field& field, meta::Reference reference);

template<class T>
bool Deserialize(std::istream& in, T& object) {
    static const meta::Type type = meta::GetType<T>();
    static const std::vector<meta::Field>& fields = type.GetFields();

    using json = nlohmann::json;
    json data;
    in >> data;
    if (data["type"] != type.FullName())
        return false;

    meta::Reference ref(object);
    for (const auto& field : fields) {
        DeserializeField(data["data"], field, ref);
    }

    return true;
}

template <class T>
using is_primitive_type = std::integral_constant<
        bool,
        std::is_same<T, bool>::value ||
        std::is_same<T, char>::value ||
        std::is_same<T, unsigned char>::value ||
        std::is_same<T, short>::value ||
        std::is_same<T, unsigned short>::value ||
        std::is_same<T, int>::value ||
        std::is_same<T, unsigned int>::value ||
        std::is_same<T, long>::value ||
        std::is_same<T, unsigned long>::value ||
        std::is_same<T, long long>::value ||
        std::is_same<T, unsigned long long>::value ||
        std::is_same<T, float>::value ||
        std::is_same<T, double>::value
    >;

nlohmann::json Serialize(const meta::Type& type, meta::Reference instance);

template<class T>
void Serialize(std::ostream& out, const T& object) {
    using json = nlohmann::json;
    static const meta::Type type = meta::GetType<T>();

    json data;
    data["type"] = type.FullName();
    data["data"] = Serialize(type, meta::Reference(object));

    out << std::setw(2) << data;
}


}