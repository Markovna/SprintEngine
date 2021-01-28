#include "transform_component.h"

namespace sprint {

TransformComponent::~TransformComponent() {
    while (child_count_) {
        registry_.destroy(child_first_);
    }

    if (parent_ != ecs::null) {
        get(parent_).EraseChild(entity_);
    }
}

void TransformComponent::SetParent(ecs::entity_t parent) {
    if (parent_ != ecs::null) {
        get(parent_).EraseChild(entity_);
    }

    if (parent != ecs::null) {
        get(parent).PushChild(entity_);
    }

    dirty_ = true;
}

void TransformComponent::SetTransform(const Transform &transform) {

    if (parent_ != ecs::null) {
        auto& parent = get(parent_);
        auto inv_parent = Transform::Inverse(parent.GetTransform());
        local_ = inv_parent * transform;
    } else {
        local_ = transform;
    }

    world_ = transform;
    dirty_ = false;
}

void TransformComponent::EraseChild(ecs::entity_t e) {
    auto& child = get(e);
    assert(child.parent_ == entity_);

    child_count_--;
    if (child_first_ == e) {
        child_first_ = child.next_;
    }

    if (child.prev_ != ecs::null) {
        get(child.prev_).next_ = child.next_;
    }

    if (child.next_ != ecs::null) {
        get(child.next_).prev_ = child.prev_;
    }

    child.parent_ = ecs::null;
    child.next_ = ecs::null;
    child.prev_ = ecs::null;
}

void TransformComponent::PushChild(ecs::entity_t e) {
    auto& child = get(e);

    if (child_first_ != ecs::null) {
        get(child_first_).prev_ = e;
    }

    child.parent_ = entity_;
    child.prev_ = ecs::null;
    child.next_ = child_first_;

    child_first_ = e;
    child_count_++;
}

void TransformComponent::Resolve() const {
    if (dirty_) {
        world_ = parent_ != ecs::null ? get(parent_).GetTransform() * local_ : local_;
        dirty_ = false;
    }
}

const Transform &TransformComponent::GetTransform() const {
    Resolve();
    return world_;
}

void TransformComponent::SetLocalTransform(const Transform &transform) {
    local_ = transform;
    dirty_ = true;
}

}