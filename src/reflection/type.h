#pragma once

#include "type_ids.h"

namespace meta {

class Field;
class Method;

class Type {
public:
    static const Type& Invalid() {
        static const Type invalid = Type(InvalidTypeId);
        return invalid;
    }

public:
    explicit Type(TypeId id) noexcept : id_(id) {};

    [[nodiscard]] const std::string& FullName() const;
    [[nodiscard]] const std::string& Name() const;
    [[nodiscard]] const std::vector<Field>& GetFields() const;
    [[nodiscard]] const Field& GetField(const std::string& name) const;
    [[nodiscard]] const std::vector<Method>& GetMethods() const;
    [[nodiscard]] const Method& GetMethod(const std::string& name) const;
    [[nodiscard]] bool Valid() const;
    [[nodiscard]] bool IsPointer() const;

    [[nodiscard]] TypeId ID() const { return id_; }

    template<class T>
    [[nodiscard]] bool Is() const { return details::GetTypeId<T>() == id_; }

    bool operator==(const Type& other) const {
        return id_ == other.id_;
    }

private:
    TypeId id_;
};

template<class T>
Type GetType() {
    return Type(details::GetTypeId<T>());
}

Type GetType(const std::string& name);

}