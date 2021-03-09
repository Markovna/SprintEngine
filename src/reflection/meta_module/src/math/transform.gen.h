#include "../../../../math/transform.h"

template<>
class ::meta::details::Reflection<sprint::Transform> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::Transform>("sprint::Transform");
        ::meta::details::Register<sprint::Transform*>("sprint::Transform");
    }

    static void InitFields() {
        ::meta::details::AddField<sprint::Transform, sprint::vector<3>>("position_", &get_position_);
        ::meta::details::AddField<sprint::Transform, sprint::Quat>("rotation_", &get_rotation_);
        ::meta::details::AddField<sprint::Transform, sprint::vector<3>>("scale_", &get_scale_);
    }

    static Reference get_position_(Reference& instance) {
        return Reference(instance.Get<sprint::Transform>().position_);
    }

    static Reference get_rotation_(Reference& instance) {
        return Reference(instance.Get<sprint::Transform>().rotation_);
    }

    static Reference get_scale_(Reference& instance) {
        return Reference(instance.Get<sprint::Transform>().scale_);
    }
};
