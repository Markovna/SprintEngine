#pragma once

#include "common/ecs/ecs.h"
#include "components/transform_component.h"

namespace sprint {

class World : public ecs::registry {
private:
    using registry = ecs::registry;
    using entity_t = ecs::entity_t;

public:
    static std::unique_ptr<World> Create();

    World();
    ~World() = default;

    ecs::entity_t CreateEntity(const Transform& local = {}, ecs::entity_t parent = ecs::null);
    void DestroyEntity(ecs::entity_t);

    TransformComponent::iterator GetRoots();
    TransformComponent::const_iterator GetRoots() const;
    size_t GetRootsSize() const;

private:
    TransformComponent root_;
};

}