#include <components/mesh_renderer.h>
#include <components/camera.h>
#include "properties_gui.h"
#include "gui.h"
#include "editor_gui.h"

namespace sprint::editor {

std::unique_ptr<PropertiesEditorGui> PropertiesEditorGui::Create(EditorGui& editor, Engine& engine) {
    return std::make_unique<PropertiesEditorGui>(editor, engine);
}

static bool Draw(meta::Reference& instance, const meta::Field& field) {
    meta::Type type(field.GetType());
    meta::Reference ref(field.GetReference(instance));

    gui::PushID(field.GetName().c_str());

    bool changed = false;

    if (!type.Valid()) {

    }
    else if (type.Is<int>() ||
        type.Is<uint32_t>() ||
        type.Is<size_t>()) {
        auto& value = ref.Get<int>();
        changed = gui::IntField(field.GetName().c_str(), &value);
    } else if (type.Is<float>()) {
        auto& value = ref.Get<float>();
        changed = gui::FloatField(field.GetName().c_str(), &value);
    } else if (type.Is<double>()) {
        auto& value = ref.Get<double>();
        changed = gui::DoubleField(field.GetName().c_str(), &value);
    } else if (type.Is<bool>()) {
        auto& value = ref.Get<bool>();
        changed = gui::BoolField(field.GetName().c_str(), &value);
    } else if (type.Is<vec4>()) {
        auto& value = ref.Get<vec4>();
        changed = gui::Vector4Field(field.GetName().c_str(), &value.x, &value.y, &value.z, &value.w);
    } else if (type.Is<Color>()) {
        auto& value = ref.Get<Color>();
        changed = gui::Vector4Field(field.GetName().c_str(), &value.r, &value.g, &value.b, &value.a);
    } else if (type.Is<vec3>()) {
        auto& value = ref.Get<vec3>();
        changed = gui::Vector3Field(field.GetName().c_str(), &value.x, &value.y, &value.z);
    } else if (type.Is<vec2>()) {
        auto& value = ref.Get<vec2>();
        changed = gui::Vector2Field(field.GetName().c_str(), &value.x, &value.y);
    }
    else {
        gui::Text("%s", field.GetName().c_str());
        gui::Indent(20.0f);
        for (const auto& inner_field : type.GetFields()) {
            changed |= Draw(ref, inner_field);
        }
        gui::Unindent(20.0f);
    }

    gui::PopID();
    return changed;
}

template<class T>
bool DrawComponent(Scene* scene, ecs::entity_t selected) {
    meta::Type type = meta::GetType<T>();

    bool node_open = gui::TreeNodeEx(type.FullName().c_str(), ImGuiTreeNodeFlags_FramePadding, "%s", type.Name().c_str());
    if (node_open) {
        gui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        meta::Reference ref(scene->get<T>(selected));
        for (const auto &field : type.GetFields()) {
            Draw(ref, field);
        }
        gui::PopStyleVar();
        gui::TreePop();
    }
    return node_open;
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

                    DrawComponent<TransformComponent>(scene, selected);

                    meta::Reference ref(scene->get<TransformComponent>(selected));
                    auto& transform_field = type.GetField("local_");
                    bool changed = Draw(ref, transform_field);

                    if (changed) {
                        auto& method = type.GetMethod("SetDirty");
                        method.Invoke(ref, {true});
                    }

                    gui::PopStyleVar();
                    gui::TreePop();
                    opened = true;
                }
            } else if (type.Is<MeshRenderer>()) {
                opened = DrawComponent<MeshRenderer>(scene, selected);
            } else if (type.Is<Camera>()) {
                opened = DrawComponent<Camera>(scene, selected);
            }
        });

        gui::Separator();
        gui::Button("+ Add Component");
    }

    ImGui::End();
}

}