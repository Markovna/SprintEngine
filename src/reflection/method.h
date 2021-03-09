#pragma once

#include "type.h"
#include "object.h"

namespace meta {

class Reference;

class Method {
private:
    using InvokePtr = Object (*)(Reference&, std::initializer_list<Object>);

public:
    static const Method& Invalid();

public:
    Method(std::string name, Type owner_type, Type ret_type, std::vector<Type> arg_types, InvokePtr invoke_ptr)
        : name_(std::move(name))
        , owner_type_(owner_type)
        , return_type_(ret_type)
        , arg_types_(arg_types)
        , invoke_ptr_(invoke_ptr)
    {}

    Object Invoke(Reference& instance, std::initializer_list<Object> args) const;

    template<class ReturnType, class ...Args>
    ReturnType operator()(Reference& instance, Args&&... args) const;

    [[nodiscard]] const std::string& GetName() const { return name_; }

    [[nodiscard]] bool IsValid() const {
        return !name_.empty() &&
                owner_type_.Valid() &&
                return_type_.Valid() &&
                std::all_of(
                    arg_types_.begin(),
                    arg_types_.end(),
                    [](const Type& type){ return type.Valid(); }) &&
                invoke_ptr_;
    }

private:
    std::string name_;
    Type owner_type_;
    Type return_type_;
    std::vector<Type> arg_types_;
    InvokePtr invoke_ptr_;
};

template<class ReturnType, class... Args>
ReturnType Method::operator()(Reference &instance, Args &&... args) const {
    if constexpr (std::is_same<ReturnType, void>::value) {
        Invoke(instance, {std::forward<Args>(args)...});
        return;
    }
    else {
        return Invoke(instance, {std::forward<Args>(args)...});
    }
}

}