#pragma once

#include "common/ecs/ecs.h"
#include "math/transform.h"
#include "reflection/meta_runtime.h"

#include <iostream>

namespace sprint {

struct SERIALIZED TransformComponent {
public:
    template<class T>
    struct child_iterator {
    private:
        using value_type = T;
        using pointer = value_type*;
        using reference = value_type&;
    public:
        explicit child_iterator(pointer curr) : curr_(curr) {}

        explicit operator pointer() noexcept {
            return curr_;
        }

        explicit operator bool() noexcept { return curr_ != nullptr; }

        child_iterator& operator++() {
            curr_ = curr_->next_ != ecs::null ? &curr_->get(curr_->next_) : nullptr;
            return *this;
        };

        child_iterator& operator--() {
            curr_ = curr_->prev_ != ecs::null ? &curr_->get(curr_->prev_) : nullptr;
            return *this;
        };

        child_iterator operator++(int) {
            child_iterator orig = *this;
            return ++(*this), orig;
        };

        child_iterator operator--(int) {
            child_iterator orig = *this;
            return --(*this), orig;
        };

        [[nodiscard]] bool operator==(const child_iterator& other) const noexcept {
            return other.curr_ == curr_;
        }

        [[nodiscard]] bool operator!=(const child_iterator& other) const noexcept {
            return !(*this == other);
        }

        [[nodiscard]] bool operator==(std::nullptr_t) const noexcept {
            return curr_ == nullptr;
        }

        [[nodiscard]] bool operator!=(std::nullptr_t) const noexcept {
            return curr_ != nullptr;
        }

        [[nodiscard]] pointer operator->() const noexcept {
            return curr_;
        }

        [[nodiscard]] reference operator*() const {
            return *curr_;
        }
    private:
        pointer curr_;
    };

    using iterator = child_iterator<TransformComponent>;
    using const_iterator = child_iterator<const TransformComponent>;

public:

    static TransformComponent& Create(
        ecs::entity_t entity,
        ecs::registry& registry,
        TransformComponent* root,
        TransformComponent* parent = nullptr,
        const Transform& local = {}
    ) {
        ecs::entity_t parent_ent = parent ? parent->entity_ : ecs::null;
        auto& component = registry.emplace<TransformComponent>(entity, registry, root, entity);
        PushChild(registry, root, component);
        if (parent_ent != ecs::null)
            component.SetParent(&registry.get<TransformComponent>(parent_ent));
        component.SetLocalTransform(local);
        return component;
    }

    TransformComponent(ecs::registry& registry, TransformComponent* root, ecs::entity_t entity) noexcept
        : registry_(registry),
          root_(root),
          entity_(entity),
          parent_(ecs::null),
          child_first_(ecs::null),
          child_last_(ecs::null),
          next_(ecs::null),
          prev_(ecs::null),
          child_count_(0),
          world_{},
          local_{},
          dirty_(false) {
    }

    ~TransformComponent();

    void SetParent(TransformComponent* parent, TransformComponent* next = nullptr);

    [[nodiscard]] TransformComponent* GetParent();
    [[nodiscard]] const TransformComponent* GetParent() const;
    [[nodiscard]] TransformComponent* GetNext();
    [[nodiscard]] const TransformComponent* GetNext() const;

    void SetTransform(const Transform& transform);
    void SetLocalTransform(const Transform& transform);

    void SetLocalPosition(const vec3& position);
    void SetLocalRotation(const quat& rotation);
    void SetLocalScale(const vec3& scale);

    const vec3& GetLocalPosition() const;
    const quat& GetLocalRotation() const;
    const vec3& GetLocalScale() const;

    iterator GetChildren();
    const_iterator GetChildren() const;
    size_t GetChildrenSize() const;

    [[nodiscard]] ecs::entity_t GetID() const;
    [[nodiscard]] const Transform& GetTransform() const;
    [[nodiscard]] const Transform& GetLocalTransform() const;
    [[nodiscard]] bool IsChildOf(const TransformComponent& parent) const;

private:
    [[nodiscard]] TransformComponent& get(ecs::entity_t e);
    [[nodiscard]] const TransformComponent& get(ecs::entity_t e) const;

    void Resolve() const;
    void EraseChild(ecs::entity_t e);
    void PushChild(ecs::entity_t e);
    void InsertChild(ecs::entity_t pos, ecs::entity_t e);
    SERIALIZABLE void SetDirty(bool) const;
    static void PushChild(ecs::registry&, TransformComponent* parent, TransformComponent& child);
    static void InsertChild(ecs::registry&, TransformComponent* parent, TransformComponent& child, TransformComponent& next);
    static bool IsChildOf(const TransformComponent& child, const TransformComponent& parent);

private:
    SERIALIZABLE ecs::registry& registry_;
    SERIALIZABLE TransformComponent* root_;
    SERIALIZABLE ecs::entity_t entity_;
    SERIALIZABLE ecs::entity_t parent_;
    SERIALIZABLE ecs::entity_t child_first_;
    SERIALIZABLE ecs::entity_t child_last_;
    SERIALIZABLE ecs::entity_t next_;
    SERIALIZABLE ecs::entity_t prev_;
    SERIALIZABLE Transform local_;
    mutable Transform world_;
    SERIALIZABLE size_t child_count_;
    SERIALIZABLE mutable bool dirty_;
};

}