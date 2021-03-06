#include <components/mesh_renderer.h>
#include <components/camera.h>
#include "properties_gui.h"
#include "components_gui.h"
#include "gui.h"
#include "editor_gui.h"

namespace sprint::editor {

std::unique_ptr<PropertiesEditorGui> PropertiesEditorGui::Create(EditorGui& editor, Engine& engine) {
    return std::make_unique<PropertiesEditorGui>(editor, engine);
}

void PropertiesEditorGui::OnGui() {

    ImGui::Begin("Inspector");

    gui::ShowDemoWindow();

    auto selected = editor_.Selected();
    if (selected != ecs::null) {

        gui::BeginGroup();
        gui::FieldLabel("Entity");
        gui::TextDisabled("ID: %u", selected);
        gui::EndGroup();

        // TODO: entity name
        static char buf[32] = "";
        gui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        gui::TextField("Name", buf, (int)(sizeof(buf) / sizeof(*(buf))));
        gui::PopStyleVar();

        Scene *scene = engine_.get_scene();
        bool opened = false;
        scene->visit(selected, [&] (const meta::TypeId id) {
            meta::Type type(id);

            opened = false;
            if (type.Is<TransformComponent>()) {
                opened = gui::DrawComponent<TransformComponent>(scene, selected);
            } else if (type.Is<MeshRenderer>()) {
                opened = gui::DrawComponent<MeshRenderer>(scene, selected);
            } else if (type.Is<Camera>()) {
                opened = gui::DrawComponent<Camera>(scene, selected);
            }
        });

        if (opened) gui::Separator();

        gui::Button("+ Component");
    }

    ImGui::End();
}

}