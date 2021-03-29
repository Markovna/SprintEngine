#pragma once

#include "common/ecs/ecs.h"

namespace sprint {

struct SERIALIZED Entity {

    uint32_t id = ecs::null;

    [[nodiscard]] bool IsValid() const { return id != ecs::null; }

    static Entity Invalid() {
        static const Entity invalid{};
        return invalid;
    }

    bool operator==(Entity other) const { return id == other.id; }
    bool operator!=(Entity other) const { return !(*this == other); }

    explicit operator bool() const { return IsValid(); }
};

}