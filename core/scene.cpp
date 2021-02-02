#include "scene.h"

namespace sprint {

std::unique_ptr<Scene> Scene::Create() {
    return std::make_unique<Scene>();
}

ecs::entity_t Scene::CreateEntity(const Transform &local, TransformComponent *parent) {
    auto entity = registry::create();
    TransformComponent::Create(entity, *this, &root_, parent, local);
//    auto transform_component = registry::emplace<TransformComponent>(entity, *this, &root_, entity);
//    if (parent)
//        transform_component.SetParent(parent);
//    transform_component.SetLocalTransform(local);
    return entity;
}

Scene::Scene()
    : root_{*this, nullptr, ecs::null} {}

TransformComponent::iterator Scene::GetRoots() {
    return root_.GetChildren();
}

TransformComponent::const_iterator Scene::GetRoots() const {
    return root_.GetChildren();
}

size_t Scene::GetRootsSize() const {
    return root_.GetChildrenSize();
}

}