#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

#define REGISTER_TYPE(__type) \
        details::Registry().Register<__type>(#__type, false); \
        details::Registry().Register<__type*>(#__type, true);

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

namespace meta {

using TypeId = uint32_t;

static constexpr TypeId InvalidTypeId = std::numeric_limits<TypeId>::max();

class Field;

namespace details {

class TypeInfo {
public:
    static const TypeInfo Invalid;

    TypeId id = InvalidTypeId;
    std::string name{};
    std::vector<Field> fields{};
    bool is_pointer;
};

template<class T>
using clean_type = std::decay_t<T>;

class MetaRegistry {
private:
    template<class T>
    struct TypeIds {
        static TypeId id;
    };

public:
    template<class T>
    static TypeId GetTypeId() { return TypeIds<clean_type<T>>::id; }

    template<class T>
    TypeId Register(const std::string& name, bool is_pointer) {
        using type = clean_type<T>;
        assert(TypeIds<type>::id == InvalidTypeId);

        TypeInfo info;
        info.id = TypeIds<type>::id = types_.size();
        info.name = name;
        info.is_pointer = is_pointer;

        if (!info.is_pointer)
            name_to_ids_[name] = info.id;

        return types_.emplace_back(std::move(info)).id;
    }

    TypeInfo& operator[](TypeId id) {
        return types_[id];
    }

    const TypeInfo& operator[](TypeId id) const {
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

inline MetaRegistry& Registry() {
    static MetaRegistry instance;
    return instance;
}

template<class T>
class Reflection;

static void RegisterPrimitiveTypes() {
    REGISTER_TYPE(bool);
    REGISTER_TYPE(char);
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

class Type {
public:
    static const Type& Invalid() {
        static const Type invalid = Type(InvalidTypeId);
        return invalid;
    }

    template<class T> static Type Get() {
        return Type(details::MetaRegistry::GetTypeId<T>());
    }

    static Type Get(const std::string& name) {
        return Type(details::Registry().GetTypeId(name));
    }

public:
    explicit Type(TypeId id) noexcept : id_(id) {};

    [[nodiscard]] const std::string& Name() const {
        return Valid() ? details::Registry()[id_].name : details::TypeInfo::Invalid.name;
    }

    [[nodiscard]] const std::vector<Field>& GetFields() const {
        return Valid() ? details::Registry()[id_].fields : details::TypeInfo::Invalid.fields;
    }

    [[nodiscard]] bool Valid() const {
        return id_ != InvalidTypeId;
    }

    [[nodiscard]] bool IsPointer() const {
        return Valid() ? details::Registry()[id_].is_pointer : false;
    }

    [[nodiscard]] TypeId Id() const {
        return id_;
    }

    template<class T>
    bool Is() { return Get<T>() == *this; }

    bool operator==(const Type& other) const {
        return id_ == other.id_;
    }

private:
    TypeId id_;
};

class Reference {
public:
    Reference() = default;

    template<class T>
    explicit Reference(
        T& data,
        typename std::enable_if<!std::is_same<Reference, T>::value>::type* = nullptr)
      : ptr_(const_cast<void*>(reinterpret_cast<const void*>(std::addressof(data))))
      , type_(Type::Get<T>())
    {}

    Reference(const Reference& other) = default;

    Reference(Reference&& other) noexcept : ptr_(other.ptr_), type_(other.type_) {
        other.ptr_ = nullptr;
        other.type_ = Type::Invalid();
    }

    Reference& operator=(const Reference& other) {
        Reference(other).Swap(*this);
        return *this;
    }

    Reference& operator=(Reference&& other) noexcept {
        other.Swap( *this );
        Reference().Swap(other);
        return *this;
    }

    template<class T>
    T& Get() const {
        return *static_cast<T*>(ptr_);
    }

    template<class T>
    [[nodiscard]] bool Is() const {
        return type_.Is<T>();
    }

    [[nodiscard]] Type GetType() const {
        return type_;
    }

private:
    void Swap(Reference& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(type_, other.type_);
    }

private:
    void* ptr_ = nullptr;
    Type type_ = Type::Invalid();
};

class Field {
private:
    using GetterPtr = Reference (*)(Reference&);

public:
    Field(std::string name, Type type, Type owner_type, GetterPtr getter_) :
        name_(std::move(name)), type_(type), owner_type_(owner_type), getter_(getter_)
    {}

    [[nodiscard]] Type GetType() const { return type_; }
    [[nodiscard]] Type GetOwnerType() const { return owner_type_; }
    [[nodiscard]] const std::string& GetName() const { return name_; }

    Reference GetReference(Reference& instance) const {
        return (getter_)(instance);
    }

    template<class T>
    void SetValue(Reference& instance, T&& value) const {
        GetReference(instance).Get<T>() = std::forward<T>(value);
    }

private:
    Type type_;
    Type owner_type_;
    std::string name_;
    GetterPtr getter_;
};

namespace details {

class TypeInitializer {
protected:
    template<class T>
    static void Register(std::string name, bool is_pointer = false) {
        Registry().Register<T>(std::move(name), is_pointer);
    }

    template<class T, class FieldType>
    static void AddField(std::string name, Reference (*getter)(Reference& instance)) {
        if (Type::Get<FieldType>().Valid()) {
            Type type = Type::Get<T>();
            Registry()[type.Id()].fields.emplace_back(
                    std::move(name), Type::Get<FieldType>(), type, getter
                );
        }
    }
};

}

}
