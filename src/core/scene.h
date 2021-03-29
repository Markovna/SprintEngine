#pragma once

#include "common/ecs/ecs.h"
#include "components/transform_component.h"
#include "entity.h"

namespace sprint {

class World;

//class SceneRootComponent { };

class SERIALIZED LinkComponent {
public:
    LinkComponent() = default;

    [[nodiscard]] Entity GetParent() const { return parent_;}
    [[nodiscard]] Entity GetChild() const { return child_; }
    [[nodiscard]] Entity GetNext() const { return next_; }
    [[nodiscard]] Entity GetPrev() const { return prev_; }
    [[nodiscard]] size_t GetChildrenSize() const { return children_size_; }

private:
    friend class World;

private:
    SERIALIZABLE Entity parent_;
    SERIALIZABLE Entity next_;
    SERIALIZABLE Entity prev_;
    SERIALIZABLE Entity child_;
    SERIALIZABLE Entity child_last_;
    SERIALIZABLE size_t children_size_ = 0;
};

class SERIALIZED TransformComponent {
public:
    explicit TransformComponent(const Transform& local = {})
        : local_(local)
        , world_cached_()
        , dirty_(false)
    {}

private:
    friend class World;

private:
    SERIALIZABLE Transform local_;

    mutable Transform world_cached_;
    mutable bool dirty_;
};

class World : ecs::registry {
public:
    static std::unique_ptr<World> Create() { return std::make_unique<World>(); }

    World() : root_({ecs::registry::create()}) {
        ecs::registry::emplace<TransformComponent>(root_.id);
        ecs::registry::emplace<LinkComponent>(root_.id);
    }

    Entity CreateEntity(const Transform& local = {}, Entity parent = Entity::Invalid(), Entity next = Entity::Invalid()) {
        Entity entity { ecs::registry::create() };
        ecs::registry::emplace<TransformComponent>(entity.id, local);
        ecs::registry::emplace<LinkComponent>(entity.id);

        SetParent(entity, parent, next);
        return entity;
    }

    void Destroy(Entity entity) {

        SetParentImpl(entity, Entity::Invalid(), Entity::Invalid());

        while (GetChildrenSize(entity)) {
            Destroy(GetChild(entity));
        }

        ecs::registry::destroy(entity.id);
    }

    void SetParent(Entity entity, Entity parent, Entity next = Entity::Invalid()) {
        Transform world_transform = GetWorldTransform(entity);
        SetParentImpl(entity, parent ? parent : root_, next);
        SetWorldTransform(entity, world_transform);
    }

    [[nodiscard]] Entity GetParent(Entity entity) const {
        Entity parent = ecs::registry::get<LinkComponent>(entity.id).GetParent();
        return parent == root_ ? Entity::Invalid() : parent;
    }

    [[nodiscard]] Transform GetLocalTransform(Entity entity) const {
        return ecs::registry::get<TransformComponent>(entity.id).local_;
    }

    [[nodiscard]] vec3 GetLocalPosition(Entity entity) const {
        return ecs::registry::get<TransformComponent>(entity.id).local_.get_position();
    }

    [[nodiscard]] quat GetLocalRotation(Entity entity) const {
        return ecs::registry::get<TransformComponent>(entity.id).local_.get_rotation();
    }

    void SetLocalTransform(Entity entity, const Transform& local) {
        TransformComponent& component = ecs::registry::get<TransformComponent>(entity.id);
        component.local_ = local;
        SetTransformDirty(entity, true);
    }

    void SetLocalPosition(Entity entity, const vec3& pos) {
        TransformComponent& component = ecs::registry::get<TransformComponent>(entity.id);
        component.local_.set_position(pos);
        SetTransformDirty(entity, true);
    }

    void SetLocalRotation(Entity entity, const quat& rot) {
        TransformComponent& component = ecs::registry::get<TransformComponent>(entity.id);
        component.local_.set_rotation(rot);
        SetTransformDirty(entity, true);
    }

    [[nodiscard]] Transform GetWorldTransform(Entity entity) const {
        return ResolveTransform(entity);
    }

    void SetWorldTransform(Entity entity, const Transform& world_transform) {
        Entity parent = GetParent(entity);
        Transform parent_inv = parent ? Transform::Inverse(GetWorldTransform(parent)) : Transform::Identity();

        TransformComponent& component = ecs::registry::get<TransformComponent>(entity.id);
        component.local_ = parent_inv * world_transform;
        component.world_cached_ = world_transform;
        component.dirty_ = false;
    }

    [[nodiscard]] Entity GetFirstRoot() const { return ecs::registry::get<LinkComponent>(root_.id).GetChild(); }
    [[nodiscard]] size_t GetRootsSize() const { return ecs::registry::get<LinkComponent>(root_.id).GetChildrenSize(); }

    [[nodiscard]] Entity GetChild(Entity entity) const {
        return ecs::registry::get<LinkComponent>(entity.id).GetChild();
    }

    [[nodiscard]] bool IsChildOf(Entity entity, Entity parent_candidate) const {
        if (parent_candidate == Entity::Invalid()) return true;

        while (entity) {
            const LinkComponent& comp = ecs::registry::get<LinkComponent>(entity.id);
            if (comp.GetParent() == parent_candidate)
                return true;

            entity = comp.GetParent();
        }
        return false;
    }

    [[nodiscard]] Entity GetPrev(Entity entity) const {
        return ecs::registry::get<LinkComponent>(entity.id).GetPrev();
    }

    [[nodiscard]] Entity GetNext(Entity entity) const {
        return ecs::registry::get<LinkComponent>(entity.id).GetNext();
    }

    [[nodiscard]] size_t GetChildrenSize(Entity entity) const {
        return ecs::registry::get<LinkComponent>(entity.id).GetChildrenSize();
    }

    template<class Component, class ...Args>
    Component& AddComponent(Entity entity, Args &&... args) {
        return ecs::registry::emplace<Component>(entity.id, std::forward<Args>(args)...);
    }

    template<class Component>
    void RemoveComponent(Entity entity) {
        static_assert(!std::is_same_v<Component, TransformComponent>, "Trying to remove TransformComponent");
        static_assert(!std::is_same_v<Component, LinkComponent>, "Trying to remove LinkComponent");

        ecs::registry::remove<Component>(entity.id);
    }

    template<class Component>
    Component& Get(Entity entity) {
        return ecs::registry::get<Component>(entity.id);
    }

    template<class Component>
    [[nodiscard]] const Component& Get(Entity entity) const {
        return ecs::registry::get<Component>(entity.id);
    }

    template<class Component>
    const Component* TryGet(Entity entity) const {
        return ecs::registry::try_get<Component>(entity.id);
    }

    template<class Component>
    Component* TryGet(Entity entity) {
        return ecs::registry::try_get<Component>(entity.id);
    }

    template<class Component>
    [[nodiscard]] bool Has(Entity entity) const {
        return ecs::registry::has<Component>(entity.id);
    }

    template<class ...Component>
    ecs::component_view<Component...> View() {
        return ecs::registry::view<Component...>();
    }

    template<typename Func>
    void Visit(Entity entity, Func func) const {
        ecs::registry::visit(entity.id, func);
    }

private:

    void SetParentImpl(Entity entity, Entity parent, Entity next);

    const Transform& ResolveTransform(Entity entity) const {
        const TransformComponent& component = ecs::registry::get<TransformComponent>(entity.id);
        if (component.dirty_) {
            Entity parent = GetParent(entity);

            component.world_cached_ = (parent ? GetWorldTransform(parent) : Transform::Identity()) * component.local_;
            component.dirty_ = false;
        }
        return component.world_cached_;
    }

    void SetTransformDirty(Entity entity, bool dirty) const {
        ecs::registry::get<TransformComponent>(entity.id).dirty_ = dirty;

        if (dirty) {
            Entity child = GetChild(entity);
            while (child) {
                SetTransformDirty(child, dirty);
                child = GetNext(child);
            }
        }
    }

private:
    Entity root_;
};

// POD data that is obtained by deserialization .scene file.
// Basically, it contains instructions to spawn entities
// and components in the World.
class SceneResource {


};

//void Spawn(World* world, SceneResource scene) {
    // iterate through entities and create each of them
    // world->CreateEntity();

    // iterate through components of each type and spawn them
    // world->AddComponent<Component>(entity)

    // set up entities hierarchy
    // world->SetParent(entity, parent)
//}

}