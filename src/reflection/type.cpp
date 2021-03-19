#include "type.h"
#include "meta_registry.h"

namespace meta {

const std::string& Type::FullName() const {
    return details::GetTypeInfo(id_).FullName();
}

const std::string &Type::Name() const {
    return details::GetTypeInfo(id_).Name();
}

const std::vector<Field> &Type::GetFields() const {
    return details::GetTypeInfo(id_).GetFields();
}

const std::vector<Method>& Type::GetMethods() const {
    return details::GetTypeInfo(id_).GetMethods();
}

bool Type::IsValid() const {
    return id_ != InvalidTypeId;
}

bool Type::IsPointer() const {
    return details::GetTypeInfo(id_).IsPointer();
}

const Field &Type::GetField(const std::string &name) const {
    return details::GetTypeInfo(id_).GetField(name);
}

const Method &Type::GetMethod(const std::string &name) const {
    return details::GetTypeInfo(id_).GetMethod(name);
}

Type GetType(const std::string &name) {
    return Type(details::GetTypeInfo(name).ID());
}

}