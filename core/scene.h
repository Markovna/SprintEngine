#pragma once

#include "../common/ecs/ecs.h"
#include "components/transform_component.h"

namespace sprint {

class Scene : public ecs::registry {
private:
    using registry = ecs::registry;
    using entity_t = ecs::entity_t;

public:
    static std::unique_ptr<Scene> Create();

    Scene() = default;
    ~Scene() = default;

    ecs::entity_t CreateEntity(const Transform& local = {}, ecs::entity_t parent = ecs::null) {
        auto entity = registry::create();
        auto transform_component = registry::emplace<TransformComponent>(entity, *this, entity);
        transform_component.SetParent(parent);
        transform_component.SetLocalTransform(local);
        return entity;
    }
};

}