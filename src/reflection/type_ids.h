#pragma once

namespace meta {

using TypeId = uint32_t;
static constexpr TypeId InvalidTypeId = std::numeric_limits<TypeId>::max();

}

namespace meta::details {

template<class T>
struct TypeIds {
    static TypeId id;
};

template<class T>
TypeId TypeIds<T>::id = InvalidTypeId;

template<class T>
TypeId GetTypeId() { return TypeIds<std::decay_t<T>>::id; }

}