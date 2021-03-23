#include "scene.h"

namespace sprint {

std::unique_ptr<World> World::Create() {
    return std::make_unique<World>();
}

World::World()
    : root_{this, nullptr, ecs::null} {}

TransformComponent::iterator World::GetRoots() {
    return root_.GetChildren();
}

TransformComponent::const_iterator World::GetRoots() const {
    return root_.GetChildren();
}

size_t World::GetRootsSize() const {
    return root_.GetChildrenSize();
}

ecs::entity_t World::CreateEntity(const Transform &local, ecs::entity_t parent) {
    auto entity = registry::create();
    TransformComponent::Create(entity, this, &root_, parent != ecs::null ? &registry::get<TransformComponent>(parent) : nullptr, local);
    return entity;
}

void World::DestroyEntity(ecs::entity_t e) {
    TransformComponent& transform = registry::get<TransformComponent>(e);
    transform.SetParent(nullptr);

    while (transform.GetChildrenSize()) {
        DestroyEntity(transform.GetChildren()->GetID());
    }

    root_.EraseChild(e);

    registry::destroy(e);
}

}