#include <components/mesh_component.h>
#include "properties_gui.h"
#include "gui.h"
#include "editor_gui.h"

namespace sprint::editor {

std::unique_ptr<PropertiesEditorGui> PropertiesEditorGui::Create(EditorGui& editor, Engine& engine) {
    return std::make_unique<PropertiesEditorGui>(editor, engine);
}

static void Draw(meta::Reference& instance, const meta::Field& field) {
    meta::Type type(field.GetType());
    meta::Reference ref(field.GetReference(instance));

    gui::PushID(field.GetName().c_str());

    if (type.Is<int>() ||
        type.Is<uint32_t>() ||
        type.Is<size_t>()) {
        auto& value = ref.Get<int>();
        gui::IntField(field.GetName().c_str(), &value);
    } else if (type.Is<float>()) {
        auto& value = ref.Get<float>();
        gui::FloatField(field.GetName().c_str(), &value);
    } else if (type.Is<double>()) {
        auto& value = ref.Get<double>();
        gui::DoubleField(field.GetName().c_str(), &value);
    } else if (type.Is<bool>()) {
        auto& value = ref.Get<bool>();
        gui::BoolField(field.GetName().c_str(), &value);
    } else if (type.Is<vec3>()) {
        auto& value = ref.Get<vec3>();
        gui::Vector3Field(field.GetName().c_str(), &value.x, &value.y, &value.z);
    }
    else {
        gui::Text("%s", field.GetName().c_str());
        gui::Indent(20.0f);
        for (const auto& inner_field : type.GetFields()) {
            Draw(ref, inner_field);
        }
        gui::Unindent(20.0f);
    }

    gui::PopID();
}

void PropertiesEditorGui::OnGui() {

    ImGui::Begin("Inspector");

    gui::ShowDemoWindow();

    auto selected = editor_.Selected();
    if (selected != ecs::null) {
        static char buf[32] = "";
        gui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        gui::TextField("Name", buf, (int)(sizeof(buf) / sizeof(*(buf))));
        gui::PopStyleVar();
        gui::Separator();

        Scene *scene = engine_.get_scene();
        bool opened = false;
        scene->visit(selected, [&] (const meta::TypeId id) {
            meta::Type type(id);

            if (opened) gui::Separator();
            opened = false;
            if (type.Is<TransformComponent>()) {
                static const char* name = "Transform";
                bool node_open = gui::TreeNodeEx(type.FullName().c_str(), ImGuiTreeNodeFlags_FramePadding, "%s", name);
                if (node_open) {
                    gui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));

                    meta::Reference ref(scene->get<TransformComponent>(selected));
                    auto& fields = type.GetFields();
                    auto transform_field = std::find_if(fields.begin(), fields.end(), [] (const meta::Field& f) { return f.GetName() == "local_"; });

                    meta::Reference transform_ref(transform_field->GetReference(ref));
                    Draw(transform_ref, *transform_field);

                    gui::PopStyleVar();
                    gui::TreePop();
                    opened = true;
                }
            } else if (type.Is<MeshComponent>()) {
                bool node_open = gui::TreeNodeEx(type.FullName().c_str(), ImGuiTreeNodeFlags_FramePadding, "%s", type.Name().c_str());
                if (node_open) {
                    gui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
                    meta::Reference ref(scene->get<MeshComponent>(selected));
                    for (const auto &field : type.GetFields()) {
                        Draw(ref, field);
                    }
                    gui::PopStyleVar();
                    gui::TreePop();
                    opened = true;
                }
            }
        });

        gui::Separator();
        gui::Button("+ Add Component");
    }

    ImGui::End();
}

}