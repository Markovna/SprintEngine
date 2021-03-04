#include "../../../../core/components/camera.h"

template<>
class ::meta::details::Reflection<sprint::Camera> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::Camera>("sprint::Camera");
        ::meta::details::Register<sprint::Camera*>("sprint::Camera");
    }

    static void InitFields() {
        ::meta::details::AddField<sprint::Camera, float>("fov_", &get_fov_);
        ::meta::details::AddField<sprint::Camera, float>("near_", &get_near_);
        ::meta::details::AddField<sprint::Camera, float>("far_", &get_far_);
        ::meta::details::AddField<sprint::Camera, float>("orthogonal_size_", &get_orthogonal_size_);
        ::meta::details::AddField<sprint::Camera, sprint::Rect>("normalized_rect_", &get_normalized_rect_);
        ::meta::details::AddField<sprint::Camera, sprint::Color>("clear_color_", &get_clear_color_);
        ::meta::details::AddField<sprint::Camera, unsigned char>("clear_flag_", &get_clear_flag_);
    }

    static Reference get_fov_(Reference& instance) {
        return Reference(instance.Get<sprint::Camera>().fov_);
    }

    static Reference get_near_(Reference& instance) {
        return Reference(instance.Get<sprint::Camera>().near_);
    }

    static Reference get_far_(Reference& instance) {
        return Reference(instance.Get<sprint::Camera>().far_);
    }

    static Reference get_orthogonal_size_(Reference& instance) {
        return Reference(instance.Get<sprint::Camera>().orthogonal_size_);
    }

    static Reference get_normalized_rect_(Reference& instance) {
        return Reference(instance.Get<sprint::Camera>().normalized_rect_);
    }

    static Reference get_clear_color_(Reference& instance) {
        return Reference(instance.Get<sprint::Camera>().clear_color_);
    }

    static Reference get_clear_flag_(Reference& instance) {
        return Reference(instance.Get<sprint::Camera>().clear_flag_);
    }
};
