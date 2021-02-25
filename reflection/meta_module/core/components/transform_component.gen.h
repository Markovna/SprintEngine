#include "../../../../core/components/transform_component.h"

template<>
class ::meta::details::Reflection<sprint::TransformComponent>
   : public ::meta::details::TypeInitializer {
public:
    static void Allocate() {
        Register<sprint::TransformComponent>("sprint::TransformComponent");
        Register<sprint::TransformComponent*>("sprint::TransformComponent", true);
    }

    static void InitFields() {
        AddField<sprint::TransformComponent, sprint::ecs::registry &>("registry_", &get_registry_);
        AddField<sprint::TransformComponent, sprint::TransformComponent *>("root_", &get_root_);
        AddField<sprint::TransformComponent, unsigned int>("entity_", &get_entity_);
        AddField<sprint::TransformComponent, unsigned int>("parent_", &get_parent_);
        AddField<sprint::TransformComponent, unsigned int>("child_first_", &get_child_first_);
        AddField<sprint::TransformComponent, unsigned int>("child_last_", &get_child_last_);
        AddField<sprint::TransformComponent, unsigned int>("next_", &get_next_);
        AddField<sprint::TransformComponent, unsigned int>("prev_", &get_prev_);
        AddField<sprint::TransformComponent, sprint::Transform>("local_", &get_local_);
        AddField<sprint::TransformComponent, unsigned long>("child_count_", &get_child_count_);
        AddField<sprint::TransformComponent, bool>("dirty_", &get_dirty_);
    }

    static Reference get_registry_(Reference& instance) {
        return Reference(instance.Get<sprint::TransformComponent>().registry_);
    }

    static Reference get_root_(Reference& instance) {
        return Reference(instance.Get<sprint::TransformComponent>().root_);
    }

    static Reference get_entity_(Reference& instance) {
        return Reference(instance.Get<sprint::TransformComponent>().entity_);
    }

    static Reference get_parent_(Reference& instance) {
        return Reference(instance.Get<sprint::TransformComponent>().parent_);
    }

    static Reference get_child_first_(Reference& instance) {
        return Reference(instance.Get<sprint::TransformComponent>().child_first_);
    }

    static Reference get_child_last_(Reference& instance) {
        return Reference(instance.Get<sprint::TransformComponent>().child_last_);
    }

    static Reference get_next_(Reference& instance) {
        return Reference(instance.Get<sprint::TransformComponent>().next_);
    }

    static Reference get_prev_(Reference& instance) {
        return Reference(instance.Get<sprint::TransformComponent>().prev_);
    }

    static Reference get_local_(Reference& instance) {
        return Reference(instance.Get<sprint::TransformComponent>().local_);
    }

    static Reference get_child_count_(Reference& instance) {
        return Reference(instance.Get<sprint::TransformComponent>().child_count_);
    }

    static Reference get_dirty_(Reference& instance) {
        return Reference(instance.Get<sprint::TransformComponent>().dirty_);
    }
};
