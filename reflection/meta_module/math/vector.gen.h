#include "../../../math/vector.h"

template<>
class ::meta::details::Reflection<sprint::vector<2>>
   : public ::meta::details::TypeInitializer {
public:
    static void Allocate() {
        Register<sprint::vector<2>>("sprint::vector<2>");
        Register<sprint::vector<2>*>("sprint::vector<2>", true);
    }

    static void InitFields() {
        AddField<sprint::vector<2>, float>("x", &get_x);
        AddField<sprint::vector<2>, float>("y", &get_y);
    }

    static Reference get_x(Reference& instance) {
        return Reference(instance.Get<sprint::vector<2>>().x);
    }

    static Reference get_y(Reference& instance) {
        return Reference(instance.Get<sprint::vector<2>>().y);
    }
};

template<>
class ::meta::details::Reflection<sprint::vector<3>>
   : public ::meta::details::TypeInitializer {
public:
    static void Allocate() {
        Register<sprint::vector<3>>("sprint::vector<3>");
        Register<sprint::vector<3>*>("sprint::vector<3>", true);
    }

    static void InitFields() {
        AddField<sprint::vector<3>, float>("x", &get_x);
        AddField<sprint::vector<3>, float>("y", &get_y);
        AddField<sprint::vector<3>, float>("z", &get_z);
    }

    static Reference get_x(Reference& instance) {
        return Reference(instance.Get<sprint::vector<3>>().x);
    }

    static Reference get_y(Reference& instance) {
        return Reference(instance.Get<sprint::vector<3>>().y);
    }

    static Reference get_z(Reference& instance) {
        return Reference(instance.Get<sprint::vector<3>>().z);
    }
};
