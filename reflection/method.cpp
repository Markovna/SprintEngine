#include "method.h"

namespace meta {

Object Method::Invoke(Reference &instance, std::initializer_list<Object> args) const {
    return invoke_ptr_(instance, args);
}

const Method &Method::Invalid() {
    static const Method invalid("", Type::Invalid(), Type::Invalid(), {}, nullptr);
    return invalid;
}

}