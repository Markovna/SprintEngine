#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#define REGISTER_TYPE(__TYPE, ...) \
            auto& __info ## __TYPE = ::sprint::meta::details::Registry().Register<__TYPE>(#__TYPE); \

#ifdef __CPP_PARSER_META__
 #define META(...) __attribute__((annotate(#__VA_ARGS__)))
#else
 #define META(...)
#endif

#define SERIALIZED META(__CLASS_SERIALIZED__)

#define FIELD(...) \
        template <class __Type> friend class Reflection; \
        META(__VA_ARGS__)

namespace sprint::meta {

using TypeId = uint32_t;

static constexpr TypeId InvalidTypeId = std::numeric_limits<TypeId>::max();

namespace details {

class FieldGetter {
public:
    void* Get(const void*);
};

class FieldInfo {
    std::string name;
};

class TypeInfo {
public:
    TypeId id{};
    std::string name;
    std::vector<FieldInfo> fields;
};

template<class ...Args>
static void AddFields(TypeInfo& type_info, Args&&... fields) {
    static_assert(std::is_same<FieldInfo, std::decay_t<Args>...>::value, "Invalid argument");
    type_info.fields.emplace_back(std::forward<Args>(fields)...);
}

template<class T>
using clear_type = std::remove_reference_t<std::remove_cv_t<std::decay_t<T>>>;

class MetaRegistry {
private:
    template<class T>
    struct TypeIds {
        static TypeId id;
    };

public:
    template<class T>
    static TypeId GetTypeId() { return TypeIds<clear_type<T>>::id; }

    template<class T>
    TypeInfo& Register(const std::string& name) {
        using type = clear_type<T>;
        assert(TypeIds<type>::id == InvalidTypeId);

        TypeInfo info;
        info.id = TypeIds<type>::id = types_.size();
        info.name = name;

        name_to_ids_[name] = info.id;
        return types_.emplace_back(std::move(info));
    }

    const TypeInfo& operator[](TypeId id) const {
        assert(id < types_.size());
        return types_[id];
    }

    const TypeInfo& operator[](const std::string& name) const {
        return types_[name_to_ids_.at(name)];
    }

    [[nodiscard]] TypeId GetTypeId(const std::string& name) const {
        if (auto it = name_to_ids_.find(name); it != name_to_ids_.end()) {
            return it->second;
        }
        return InvalidTypeId;
    }

private:
    std::vector<TypeInfo> types_;
    std::unordered_map<std::string, TypeId> name_to_ids_;
};

template<class T>
TypeId MetaRegistry::TypeIds<T>::id = InvalidTypeId;

static MetaRegistry& Registry() {
    static MetaRegistry instance;
    return instance;
}

template<class T>
class Reflection {};

}

class Type {
public:
    template<class T> static Type Get() {
        return Type(details::MetaRegistry::GetTypeId<T>());
    }

    static Type Get(const std::string& name) {
        return Type(details::Registry().GetTypeId(name));
    }

public:
    [[nodiscard]] const std::string& Name() const {
        return details::Registry()[id_].name;
    }

private:
    explicit Type(TypeId id) : id_(id) {};

private:
    TypeId id_;
};

/*
class ObjectPtr {
public:
    template<class T> T& GetValue();
    template<class T> const T& GetValue() const;
};

class FieldInfo {
public:
    const Type& GetType() const;
    const Type& StorageType() const;

    template<class T> void SetValue(ObjectPtr& obj, T&&);
    ObjectPtr GetValue(const ObjectPtr& obj);
};
*/


class TestClass {
private:
    int value_;

    template<class T>
    friend class ::sprint::meta::details::Reflection;
};

template<>
class ::sprint::meta::details::Reflection<TestClass> {

    Reflection() {

    }
};

void Test() {




    REGISTER_TYPE(TestClass)

}

}
