#include "../../../../core/scene.h"

template<>
class ::meta::details::Reflection<sprint::Entity> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::Entity>("sprint::Entity");
        ::meta::details::Register<sprint::Entity*>("sprint::Entity");
    }

    static void InitFields() {
        ::meta::details::AddField<sprint::Entity, unsigned int>("id", &get_id);
    }

    static Reference get_id(Reference& instance) {
        return Reference(instance.Get<sprint::Entity>().id);
    }
};

template<>
class ::meta::details::Reflection<sprint::LinkComponent> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::LinkComponent>("sprint::LinkComponent");
        ::meta::details::Register<sprint::LinkComponent*>("sprint::LinkComponent");
    }

    static void InitFields() {
        ::meta::details::AddField<sprint::LinkComponent, sprint::Entity>("parent_", &get_parent_);
        ::meta::details::AddField<sprint::LinkComponent, sprint::Entity>("next_", &get_next_);
        ::meta::details::AddField<sprint::LinkComponent, sprint::Entity>("prev_", &get_prev_);
        ::meta::details::AddField<sprint::LinkComponent, sprint::Entity>("child_", &get_child_);
        ::meta::details::AddField<sprint::LinkComponent, sprint::Entity>("child_last_", &get_child_last_);
        ::meta::details::AddField<sprint::LinkComponent, unsigned long>("children_size_", &get_children_size_);
    }

    static Reference get_parent_(Reference& instance) {
        return Reference(instance.Get<sprint::LinkComponent>().parent_);
    }

    static Reference get_next_(Reference& instance) {
        return Reference(instance.Get<sprint::LinkComponent>().next_);
    }

    static Reference get_prev_(Reference& instance) {
        return Reference(instance.Get<sprint::LinkComponent>().prev_);
    }

    static Reference get_child_(Reference& instance) {
        return Reference(instance.Get<sprint::LinkComponent>().child_);
    }

    static Reference get_child_last_(Reference& instance) {
        return Reference(instance.Get<sprint::LinkComponent>().child_last_);
    }

    static Reference get_children_size_(Reference& instance) {
        return Reference(instance.Get<sprint::LinkComponent>().children_size_);
    }
};

template<>
class ::meta::details::Reflection<sprint::TransformComponent> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::TransformComponent>("sprint::TransformComponent");
        ::meta::details::Register<sprint::TransformComponent*>("sprint::TransformComponent");
    }

    static void InitFields() {
        ::meta::details::AddField<sprint::TransformComponent, sprint::Transform>("local_", &get_local_);
    }

    static Reference get_local_(Reference& instance) {
        return Reference(instance.Get<sprint::TransformComponent>().local_);
    }
};
