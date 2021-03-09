#include "../../../../../core/components/transform_component.h"

template<>
class ::meta::details::Reflection<sprint::TransformComponent> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::TransformComponent>("sprint::TransformComponent");
        ::meta::details::Register<sprint::TransformComponent*>("sprint::TransformComponent");
    }

    static void InitFields() {
        ::meta::details::AddField<sprint::TransformComponent, sprint::ecs::registry &>("registry_", &get_registry_);
        ::meta::details::AddField<sprint::TransformComponent, sprint::TransformComponent *>("root_", &get_root_);
        ::meta::details::AddField<sprint::TransformComponent, unsigned int>("entity_", &get_entity_);
        ::meta::details::AddField<sprint::TransformComponent, unsigned int>("parent_", &get_parent_);
        ::meta::details::AddField<sprint::TransformComponent, unsigned int>("child_first_", &get_child_first_);
        ::meta::details::AddField<sprint::TransformComponent, unsigned int>("child_last_", &get_child_last_);
        ::meta::details::AddField<sprint::TransformComponent, unsigned int>("next_", &get_next_);
        ::meta::details::AddField<sprint::TransformComponent, unsigned int>("prev_", &get_prev_);
        ::meta::details::AddField<sprint::TransformComponent, sprint::Transform>("local_", &get_local_);
        ::meta::details::AddField<sprint::TransformComponent, unsigned long>("child_count_", &get_child_count_);
        ::meta::details::AddField<sprint::TransformComponent, bool>("dirty_", &get_dirty_);

        ::meta::details::AddMethod<sprint::TransformComponent, void, bool>("SetDirty", &get_SetDirty);
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

    static Object get_SetDirty(Reference& instance, std::initializer_list<Object> args) {
        auto args_it = args.begin();
        auto& arg0 = args_it->Get<bool>(); args_it++;
        instance.Get<sprint::TransformComponent>().SetDirty(arg0);
        return {};
    }
};
