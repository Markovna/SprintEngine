#include "../../../../common/rect.h"

template<>
class ::meta::details::Reflection<sprint::Rect> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::Rect>("sprint::Rect");
        ::meta::details::Register<sprint::Rect*>("sprint::Rect");
    }

    static void InitFields() {
        ::meta::details::AddField<sprint::Rect, float>("x", &get_x);
        ::meta::details::AddField<sprint::Rect, float>("y", &get_y);
        ::meta::details::AddField<sprint::Rect, float>("width", &get_width);
        ::meta::details::AddField<sprint::Rect, float>("height", &get_height);
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
