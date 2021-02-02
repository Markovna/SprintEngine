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

    Scene();
    ~Scene() = default;

    ecs::entity_t CreateEntity(const Transform& local = {}, TransformComponent* parent = nullptr);

    TransformComponent::iterator GetRoots();
    TransformComponent::const_iterator GetRoots() const;
    size_t GetRootsSize() const;

private:
    TransformComponent root_;
};

}