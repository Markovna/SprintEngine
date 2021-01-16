#include "ecs.h"

namespace sprint {

namespace ecs {

component_id details::next_id() {
    static component_id value{};
    return value++;
}

}

}