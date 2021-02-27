#include "../../../../core/components/mesh_component.h"

template<>
class ::meta::details::Reflection<MeshComponent>
   : public ::meta::details::TypeInitializer {
public:
    static void Allocate() {
        Register<MeshComponent>("MeshComponent");
        Register<MeshComponent*>("MeshComponent", true);
    }

    static void InitFields() {
        AddField<MeshComponent, float>("float_value", &get_float_value);
        AddField<MeshComponent, int>("int_value", &get_int_value);
        AddField<MeshComponent, bool>("bool_value", &get_bool_value);
    }

    static Reference get_float_value(Reference& instance) {
        return Reference(instance.Get<MeshComponent>().float_value);
    }

    static Reference get_int_value(Reference& instance) {
        return Reference(instance.Get<MeshComponent>().int_value);
    }

    static Reference get_bool_value(Reference& instance) {
        return Reference(instance.Get<MeshComponent>().bool_value);
    }
};
