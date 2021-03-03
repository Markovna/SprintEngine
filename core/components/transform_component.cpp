#include "transform_component.h"

namespace sprint {

TransformComponent::~TransformComponent() {
    //TODO
//    while (child_count_) {
//        registry_.destroy(child_first_);
//    }

//    if (TransformComponent* parent = parent_ != ecs::null ? &get(parent_) : root_)
//        parent->EraseChild(entity_);
}

void TransformComponent::SetParent(TransformComponent* parent, TransformComponent *next) {
    if (!parent) parent = root_;
    assert(!next || next->parent_ == parent->entity_);
    assert(parent->entity_ != entity_);

    if (TransformComponent* curr_parent = parent_ != ecs::null ? &get(parent_) : root_)
        curr_parent->EraseChild(entity_);

    if (next) {
        parent->InsertChild(next->entity_, entity_);
    } else {
        parent->PushChild(entity_);
    }
    SetDirty(true);
}

void TransformComponent::SetTransform(const Transform &transform) {
    if (TransformComponent* parent = parent_ != ecs::null ? &get(parent_) : root_) {
        auto inv_parent = Transform::Inverse(parent->GetTransform());
        local_ = inv_parent * transform;
    } else {
        local_ = transform;
    }
    world_ = transform;
    SetDirty(false);
}

void TransformComponent::EraseChild(ecs::entity_t e) {
    auto& child = get(e);
    assert(child.parent_ == entity_);

    child_count_--;
    if (child_first_ == e) {
        child_first_ = child.next_;
    }

    if (child_last_ == e) {
        child_last_ = child.prev_;
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

void TransformComponent::InsertChild(ecs::entity_t pos, ecs::entity_t e) {
    assert(e != ecs::null && pos != ecs::null);
    InsertChild(registry_, this, get(e), get(pos));
}

void TransformComponent::PushChild(ecs::entity_t e) {
    assert(e != ecs::null);
    PushChild(registry_, this, get(e));
}

void TransformComponent::Resolve() const {
    if (dirty_) {
        world_ = parent_ != ecs::null ? get(parent_).GetTransform() * local_ : local_;
        SetDirty(false);
    }
}

const Transform &TransformComponent::GetTransform() const {
    Resolve();
    return world_;
}

void TransformComponent::SetLocalTransform(const Transform &transform) {
    local_ = transform;
    SetDirty(true);
}

TransformComponent* TransformComponent::GetParent() {
    return parent_ != ecs::null ? &get(parent_) : nullptr;
}

const TransformComponent* TransformComponent::GetParent() const {
    return parent_ != ecs::null ? &get(parent_) : nullptr;
}

const Transform &TransformComponent::GetLocalTransform() const {
    return local_;
}

TransformComponent &TransformComponent::get(ecs::entity_t e) {
    return registry_.get<TransformComponent>(e);
}

const TransformComponent& TransformComponent::get(ecs::entity_t e) const {
    return registry_.get<TransformComponent>(e);
}

TransformComponent::child_iterator<const TransformComponent> TransformComponent::GetChildren() const {
    return child_iterator<const TransformComponent> { child_first_ != ecs::null ? &get(child_first_) : nullptr };
}

TransformComponent::child_iterator<TransformComponent> TransformComponent::GetChildren() {
    return child_iterator<TransformComponent> { child_first_ != ecs::null ? &get(child_first_) : nullptr };
}

size_t TransformComponent::GetChildrenSize() const {
    return child_count_;
}

void TransformComponent::PushChild(ecs::registry& registry, TransformComponent *parent, TransformComponent &child) {
    if (parent) {
        if (parent->child_last_ != ecs::null) {
            registry.get<TransformComponent>(parent->child_last_).next_ = child.entity_;
        }

        child.parent_ = parent->entity_;
        child.prev_ = parent->child_last_;
        child.next_ = ecs::null;

        parent->child_last_ = child.entity_;
        if (parent->child_first_ == ecs::null)
            parent->child_first_ = parent->child_last_;

        parent->child_count_++;
    }
    else {
        child.parent_ = ecs::null;
        child.prev_ = ecs::null;
        child.next_ = ecs::null;
    }
}

void TransformComponent::InsertChild(ecs::registry& registry, TransformComponent *parent, TransformComponent &child, TransformComponent &next) {
    ecs::entity_t parent_id = parent ? parent->entity_ : ecs::null;
    assert(next.parent_ == parent_id);

    ecs::entity_t prev = next.prev_;
    if (prev != ecs::null) {
        registry.get<TransformComponent>(prev).next_ = child.entity_;
    } else {
        if (parent)
            parent->child_first_ = child.entity_;
    }

    next.prev_ = child.entity_;

    child.parent_ = parent_id;
    child.prev_ = prev;
    child.next_ = next.entity_;

    if (parent) parent->child_count_++;
}

ecs::entity_t TransformComponent::GetID() const {
    return entity_;
}

TransformComponent *TransformComponent::GetNext() {
    return next_ != ecs::null ? &get(next_) : nullptr;
}

const TransformComponent *TransformComponent::GetNext() const {
    return next_ != ecs::null ? &get(next_) : nullptr;
}

bool TransformComponent::IsChildOf(const TransformComponent &child, const TransformComponent &parent) {
    const TransformComponent* curr = &child;
    ecs::entity_t parent_id = parent.entity_;
    while (curr) {
        if (curr->entity_ == parent_id)
            return true;
        curr = curr->GetParent();
    }
    return false;
}

bool TransformComponent::IsChildOf(const TransformComponent &parent) const {
    return IsChildOf(*this, parent);
}

void TransformComponent::SetDirty(bool dirty) const {
    dirty_ = dirty;

    if (dirty_) {
        auto children = GetChildren();
        while (children != nullptr) {
            children->SetDirty(dirty_);
            children++;
        }
    }
}

}