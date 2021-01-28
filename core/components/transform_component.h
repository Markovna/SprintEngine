#pragma once

#include <ecs/ecs.h>
#include <transform.h>

namespace sprint {

struct TransformComponent {
public:
    TransformComponent(ecs::registry& registry, ecs::entity_t entity)
        noexcept
        : registry_(registry),
          entity_(entity),
          parent_(ecs::null),
          child_first_(ecs::null),
          next_(ecs::null),
          prev_(ecs::null),
          child_count_(0),
          world_{},
          local_{},
          dirty_(false) {}

    ~TransformComponent();

    void SetParent(ecs::entity_t parent);

    void SetTransform(const Transform& transform);
    void SetLocalTransform(const Transform& transform);

    [[nodiscard]] const Transform& GetTransform() const;
    [[nodiscard]] const Transform& GetLocalTransform() const { return local_; }

private:
    [[nodiscard]] TransformComponent& get(ecs::entity_t e) { return registry_.get<TransformComponent>(e); }
    [[nodiscard]] const TransformComponent& get(ecs::entity_t e) const { return registry_.get<TransformComponent>(e); }

    void Resolve() const;
    void EraseChild(ecs::entity_t e);
    void PushChild(ecs::entity_t e);

private:
    ecs::registry& registry_;
    ecs::entity_t entity_;
    ecs::entity_t parent_;
    ecs::entity_t child_first_;
    ecs::entity_t next_;
    ecs::entity_t prev_;
    size_t child_count_;
    Transform local_;
    mutable Transform world_;
    mutable bool dirty_;
};

}