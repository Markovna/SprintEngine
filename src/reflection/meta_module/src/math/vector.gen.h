#include "../../../../math/vector.h"

template<>
class ::meta::details::Reflection<sprint::vector<2>> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::vector<2>>("sprint::vector<2>");
        ::meta::details::Register<sprint::vector<2>*>("sprint::vector<2>");
    }

    static void InitFields() {
        ::meta::details::AddField<sprint::vector<2>, float>("x", &get_x);
        ::meta::details::AddField<sprint::vector<2>, float>("y", &get_y);
    }

    static Reference get_x(Reference& instance) {
        return Reference(instance.Get<sprint::vector<2>>().x);
    }

    static Reference get_y(Reference& instance) {
        return Reference(instance.Get<sprint::vector<2>>().y);
    }
};

template<>
class ::meta::details::Reflection<sprint::vector<3>> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::vector<3>>("sprint::vector<3>");
        ::meta::details::Register<sprint::vector<3>*>("sprint::vector<3>");
    }

    static void InitFields() {
        ::meta::details::AddField<sprint::vector<3>, float>("x", &get_x);
        ::meta::details::AddField<sprint::vector<3>, float>("y", &get_y);
        ::meta::details::AddField<sprint::vector<3>, float>("z", &get_z);
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
