#pragma once

#include "meta_registry.h"

#ifdef __CPP_PARSER_META__
    #define META(...) __attribute__((annotate(#__VA_ARGS__)))
#else
    #define META(...)
#endif

#define SERIALIZED META(__CLASS_SERIALIZED__)

#define SERIALIZABLE \
        template <class __Type> \
        friend class ::meta::details::Reflection; \
        META(__FIELD_SERIALIZED__)

#define REGISTER_TYPE(__type) \
        details::Register<__type>(#__type); \
        details::Register<__type*>(#__type);

namespace meta::details {

template<class T>
class Reflection;

template<class T>
void Register(const std::string& full_name) {
    MetaRegistry& registry = Registry();
    registry.AddType<T>(full_name);
}

template<class T, class FieldType>
void AddField(std::string name, Reference (*getter)(Reference& instance)) {
    Type type = GetType<T>();
    if (type.IsValid()) {
        MetaRegistry& registry = Registry();
        registry[type.ID()].AddField(
                Field(std::move(name), GetType<FieldType>(), type, getter)
            );
    }
}

template<class T, class ReturnType, class ...Args>
void AddMethod(std::string name, Object (*ptr)(Reference&, std::initializer_list<Object>)) {
    Type type = GetType<T>();
    if (type.IsValid()) {
        MetaRegistry& registry = Registry();
        registry[type.ID()].AddMethod(
                Method(std::move(name), type, GetType<ReturnType>(), { GetType<Args>()... }, ptr)
            );
    }
}

static void RegisterPrimitiveTypes() {
    REGISTER_TYPE(void);
    REGISTER_TYPE(bool);
    REGISTER_TYPE(char);
    REGISTER_TYPE(unsigned char);
    REGISTER_TYPE(short);
    REGISTER_TYPE(unsigned short);
    REGISTER_TYPE(int);
    REGISTER_TYPE(unsigned int);
    REGISTER_TYPE(long);
    REGISTER_TYPE(unsigned long);
    REGISTER_TYPE(long long);
    REGISTER_TYPE(unsigned long long);
    REGISTER_TYPE(float);
    REGISTER_TYPE(double);
}

}