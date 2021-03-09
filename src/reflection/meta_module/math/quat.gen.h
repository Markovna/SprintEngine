#include "../../../math/quat.h"

template<>
class ::meta::details::Reflection<sprint::Quat> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::Quat>("sprint::Quat");
        ::meta::details::Register<sprint::Quat*>("sprint::Quat");
    }

    static void InitFields() {
        ::meta::details::AddField<sprint::Quat, float>("x_", &get_x);
        ::meta::details::AddField<sprint::Quat, float>("y_", &get_y);
        ::meta::details::AddField<sprint::Quat, float>("z_", &get_z);
        ::meta::details::AddField<sprint::Quat, float>("w_", &get_w);
    }

    static Reference get_x(Reference& instance) {
        return Reference(instance.Get<sprint::Quat>().x_);
    }

    static Reference get_y(Reference& instance) {
        return Reference(instance.Get<sprint::Quat>().y_);
    }

    static Reference get_z(Reference& instance) {
        return Reference(instance.Get<sprint::Quat>().z_);
    }

    static Reference get_w(Reference& instance) {
        return Reference(instance.Get<sprint::Quat>().w_);
    }
};
