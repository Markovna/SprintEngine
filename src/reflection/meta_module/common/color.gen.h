#include "../../../common/color.h"

template<>
class ::meta::details::Reflection<sprint::Color> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::Color>("sprint::Color");
        ::meta::details::Register<sprint::Color*>("sprint::Color");
    }

    static void InitFields() {
        ::meta::details::AddField<sprint::Color, float>("r", &get_r);
        ::meta::details::AddField<sprint::Color, float>("g", &get_g);
        ::meta::details::AddField<sprint::Color, float>("b", &get_b);
        ::meta::details::AddField<sprint::Color, float>("a", &get_a);
    }

    static Reference get_r(Reference& instance) {
        return Reference(instance.Get<sprint::Color>().r);
    }

    static Reference get_g(Reference& instance) {
        return Reference(instance.Get<sprint::Color>().g);
    }

    static Reference get_b(Reference& instance) {
        return Reference(instance.Get<sprint::Color>().b);
    }

    static Reference get_a(Reference& instance) {
        return Reference(instance.Get<sprint::Color>().a);
    }
};
