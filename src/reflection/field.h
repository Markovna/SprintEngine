#pragma once

#include "reference.h"
#include "object.h"

namespace meta {

class Field {
private:
    using GetterPtr = Reference (*)(Reference &);

public:
    static const Field& Invalid();

public:
    Field(std::string name, Type type, Type owner_type, GetterPtr getter_) :
        name_(std::move(name)), type_(type), owner_type_(owner_type), getter_(getter_) {}

    [[nodiscard]] Type GetType() const { return type_; }
    [[nodiscard]] Type GetOwnerType() const { return owner_type_; }
    [[nodiscard]] const std::string& GetName() const { return name_; }

    Reference GetReference(Reference &instance) const {
        return (getter_)(instance);
    }

    template<class T>
    T& GetValue(Reference& instance) const {
        return GetReference(instance).Get<T>();
    }

    template<class T>
    void SetValue(Reference& instance, T &&value) const {
        GetReference(instance).Get<T>() = std::forward<T>(value);
    }

    [[nodiscard]] bool IsValid() const {
        return !name_.empty() && type_.Valid() && owner_type_.Valid() && getter_;
    }

private:
    Type type_;
    Type owner_type_;
    std::string name_;
    GetterPtr getter_;
};

}

