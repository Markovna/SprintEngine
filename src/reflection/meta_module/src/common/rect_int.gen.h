#include "../../../../common/rect_int.h"

template<>
class ::meta::details::Reflection<sprint::RectInt> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::RectInt>("sprint::RectInt");
        ::meta::details::Register<sprint::RectInt*>("sprint::RectInt");
    }

    static void InitFields() {
        ::meta::details::AddField<sprint::RectInt, int>("x", &get_x);
        ::meta::details::AddField<sprint::RectInt, int>("y", &get_y);
        ::meta::details::AddField<sprint::RectInt, int>("width", &get_width);
        ::meta::details::AddField<sprint::RectInt, int>("height", &get_height);
    }

    static Reference get_x(Reference& instance) {
        return Reference(instance.Get<sprint::RectInt>().x);
    }

    static Reference get_y(Reference& instance) {
        return Reference(instance.Get<sprint::RectInt>().y);
    }

    static Reference get_width(Reference& instance) {
        return Reference(instance.Get<sprint::RectInt>().width);
    }

    static Reference get_height(Reference& instance) {
        return Reference(instance.Get<sprint::RectInt>().height);
    }
};
