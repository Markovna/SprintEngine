#include "components_gui.h"
#include "components/camera.h"

namespace sprint::editor::gui {

template<>
void DrawComponentContent<TransformComponent>(meta::Reference ref) {
    static const meta::Type type = meta::GetType<TransformComponent>();
    static const meta::Field& transform_field = type.GetField("local_");
    static const meta::Type transform_type = transform_field.GetType();
    static const meta::Method& set_dirty_method = type.GetMethod("SetDirty");
    static const meta::Field& position_field = transform_type.GetField("position_");
    static const meta::Field& rotation_field = transform_type.GetField("rotation_");
    static const meta::Field& scale_field = transform_type.GetField("scale_");

    bool changed;
    meta::Reference transform_ref(transform_field.GetReference(ref));
    changed |= gui::DrawField(transform_ref, position_field);
    changed |= gui::DrawField(transform_ref, rotation_field);
    changed |= gui::DrawField(transform_ref, scale_field);

    if (changed) {
        set_dirty_method.Invoke(ref, {true});
    }
}

//template<>
//void DrawComponentContent<Camera>(meta::Reference ref) {
//    meta::Type type = meta::GetType<Camera>();
//
//    auto near_field = type.GetField("near_");
//    auto far_field = type.GetField("far_");
//    auto fov_field = type.GetField("fov_");
//
//    gui::BeginGroup();
//    gui::PushID("type");
//
//    auto& fov_value = fov_field.GetValue<float>(ref);
//    int item = fov_value > 0.0f ? 0 : 1;
//    bool perspective;
////    static const char* items[2] = {"Perspective", "Orthogonal"};
//    gui::FieldLabel("type");
//    if (ImGui::Combo("", &item, "Perspective\0Orthogonal\0")) {
//        perspective = item == 0;
//        fov_value = perspective ? 60.0f : -1.0f;
//    }
//
//    gui::PopID();
//    gui::EndGroup();
//
//    if (perspective) {
//        gui::DrawField(ref, fov_field);
//    } else {
//        auto ortho_size_field = type.GetField("orthogonal_size_");
//        gui::DrawField(ref, ortho_size_field);
//    }
//
//    gui::DrawField(ref, near_field);
//    gui::DrawField(ref, far_field);
//}

}