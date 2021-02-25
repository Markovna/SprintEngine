#include "../../../common/color.h"

template<>
class ::meta::details::Reflection<sprint::Color>
   : public ::meta::details::TypeInitializer {
public:
    static void Allocate() {
        Register<sprint::Color>("sprint::Color");
        Register<sprint::Color*>("sprint::Color", true);
    }

    static void InitFields() {
        AddField<sprint::Color, float>("r", &get_r);
        AddField<sprint::Color, float>("g", &get_g);
        AddField<sprint::Color, float>("b", &get_b);
        AddField<sprint::Color, float>("a", &get_a);
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
