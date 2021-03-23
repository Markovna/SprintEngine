#include "serializer.h"

template<class T>
void DeserializeFieldImpl(nlohmann::json& data, const meta::Field& field, meta::Reference reference) {
    field.SetValue<T>(reference, data[field.GetName()].get<T>());
}

template<class T>
nlohmann::json SerializeImpl(meta::Reference instance) {
    return instance.Get<T>();
}

nlohmann::json sprint::serializer::Serialize(const meta::Type& type, meta::Reference instance) {
    using method_ptr = nlohmann::json (*)(meta::Reference);
#define SERIALIZE_BASE(__type) { meta::details::GetTypeId<__type>(), SerializeImpl<__type> }
    static const std::unordered_map<meta::TypeId, method_ptr> methods {
        SERIALIZE_BASE(bool),
        SERIALIZE_BASE(char),
        SERIALIZE_BASE(unsigned char),
        SERIALIZE_BASE(short),
        SERIALIZE_BASE(unsigned short),
        SERIALIZE_BASE(int),
        SERIALIZE_BASE(unsigned int),
        SERIALIZE_BASE(long),
        SERIALIZE_BASE(unsigned long),
        SERIALIZE_BASE(long long),
        SERIALIZE_BASE(unsigned long long),
        SERIALIZE_BASE(float),
        SERIALIZE_BASE(double),
    };
#undef SERIALIZE_BASE

    if (auto it = methods.find(type.ID()); it != methods.end()) {
        return it->second(instance);
    }

    nlohmann::json data;
    for (const auto &field : instance.GetType().GetFields()) {
        data[field.GetName()] = Serialize(field.GetType(), field.GetReference(instance));
    }
    return data;
}

void sprint::serializer::DeserializeField(nlohmann::json &data, const meta::Field &field, meta::Reference reference) {
    using method_ptr = void (*)(nlohmann::json&, const meta::Field&, meta::Reference);

    meta::Type type = field.GetType();

#define SERIALIZE_BASE(__type) { meta::details::GetTypeId<__type>(), DeserializeFieldImpl<__type> }
    static const std::unordered_map<meta::TypeId, method_ptr> methods {
        SERIALIZE_BASE(bool),
        SERIALIZE_BASE(char),
        SERIALIZE_BASE(unsigned char),
        SERIALIZE_BASE(short),
        SERIALIZE_BASE(unsigned short),
        SERIALIZE_BASE(int),
        SERIALIZE_BASE(unsigned int),
        SERIALIZE_BASE(long),
        SERIALIZE_BASE(unsigned long),
        SERIALIZE_BASE(long long),
        SERIALIZE_BASE(unsigned long long),
        SERIALIZE_BASE(float),
        SERIALIZE_BASE(double),
    };
#undef SERIALIZE_BASE

    if (auto it = methods.find(type.ID()); it != methods.end()) {
        it->second(data, field, reference);
    } else {
        for (const auto& inner_field : type.GetFields()) {
            DeserializeField(data[field.GetName()], inner_field, field.GetReference(reference));
        }
    }
}
