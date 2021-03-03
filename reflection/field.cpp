#include "field.h"

namespace meta {

const Field &Field::Invalid() {
    static const Field invalid("", Type::Invalid(), Type::Invalid(), nullptr);
    return invalid;
}
}