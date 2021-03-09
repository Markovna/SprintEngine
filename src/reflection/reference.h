#pragma once

#include "type.h"

namespace meta {

class Reference {
public:
    Reference() = default;

    template<
        class T,
        class = std::enable_if_t<!std::is_same<Reference, std::decay_t<T>>::value>
    >
    explicit Reference(T& data)
        : ptr_(const_cast<void*>(reinterpret_cast<const void*>(std::addressof(data))))
        , type_(::meta::GetType<T>())
    {}

    Reference(const Reference& other) = default;

    Reference(Reference&& other) noexcept {
        Swap(other);
    }

    Reference& operator=(const Reference& other) {
        Reference(other).Swap(*this);
        return *this;
    }

    Reference& operator=(Reference&& other) noexcept {
        Reference(std::move(other)).Swap(*this);
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

}