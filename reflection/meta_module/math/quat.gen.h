#include "../../../math/quat.h"

template<>
class ::meta::details::Reflection<sprint::Quat>
   : public ::meta::details::TypeInitializer {
public:
    static void Allocate() {
        Register<sprint::Quat>("sprint::Quat");
        Register<sprint::Quat*>("sprint::Quat", true);
    }

    static void InitFields() {
        AddField<sprint::Quat, float>("x", &get_x);
        AddField<sprint::Quat, float>("y", &get_y);
        AddField<sprint::Quat, float>("z", &get_z);
        AddField<sprint::Quat, float>("w", &get_w);
    }

    static Reference get_x(Reference& instance) {
        return Reference(instance.Get<sprint::Quat>().x);
    }

    static Reference get_y(Reference& instance) {
        return Reference(instance.Get<sprint::Quat>().y);
    }

    static Reference get_z(Reference& instance) {
        return Reference(instance.Get<sprint::Quat>().z);
    }

    static Reference get_w(Reference& instance) {
        return Reference(instance.Get<sprint::Quat>().w);
    }
};
