#include "../../../common/rect.h"

template<>
class ::meta::details::Reflection<sprint::Rect>
   : public ::meta::details::TypeInitializer {
public:
    static void Allocate() {
        Register<sprint::Rect>("sprint::Rect");
        Register<sprint::Rect*>("sprint::Rect", true);
    }

    static void InitFields() {
        AddField<sprint::Rect, int>("x", &get_x);
        AddField<sprint::Rect, int>("y", &get_y);
        AddField<sprint::Rect, unsigned int>("width", &get_width);
        AddField<sprint::Rect, unsigned int>("height", &get_height);
    }

    static Reference get_x(Reference& instance) {
        return Reference(instance.Get<sprint::Rect>().x);
    }

    static Reference get_y(Reference& instance) {
        return Reference(instance.Get<sprint::Rect>().y);
    }

    static Reference get_width(Reference& instance) {
        return Reference(instance.Get<sprint::Rect>().width);
    }

    static Reference get_height(Reference& instance) {
        return Reference(instance.Get<sprint::Rect>().height);
    }
};
