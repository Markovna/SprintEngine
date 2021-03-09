#include "components_gui.h"
#include "core/components/camera.h"

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

}