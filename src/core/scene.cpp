#include "scene.h"

namespace sprint {

void World::SetParentImpl(Entity entity, Entity parent, Entity next) {
    LinkComponent& component = Get<LinkComponent>(entity);
    if (component.parent_) {
        LinkComponent& parent_comp = Get<LinkComponent>(component.parent_);
        parent_comp.children_size_--;

        if (parent_comp.child_ == entity)
            parent_comp.child_ = component.next_;

        if (parent_comp.child_last_ == entity)
            parent_comp.child_last_ = component.prev_;
    }

    if (component.prev_)
        Get<LinkComponent>(component.prev_).next_ = component.next_;

    if (component.next_)
        Get<LinkComponent>(component.next_).prev_ = component.prev_;

    component.parent_ = parent;
    component.next_ = next;
    component.prev_ = Entity::Invalid();

    if (component.parent_) {
        LinkComponent& parent_comp = Get<LinkComponent>(component.parent_);
        parent_comp.children_size_++;

        if (!component.next_) {
            if (parent_comp.child_last_) {
                LinkComponent& last_child_comp = Get<LinkComponent>(parent_comp.child_last_);
                last_child_comp.next_ = entity;
            }

            component.prev_ = parent_comp.child_last_;
            parent_comp.child_last_ = entity;
        }

        if (component.next_ == parent_comp.child_)
            parent_comp.child_ = entity;
    }

    if (component.next_) {
        LinkComponent& next_comp = Get<LinkComponent>(component.next_);

        if (next_comp.prev_) {
            LinkComponent &prev_comp = Get<LinkComponent>(next_comp.prev_);
            prev_comp.next_ = entity;
            component.prev_ = next_comp.prev_;
        }

        next_comp.prev_ = entity;
    }
}

}
