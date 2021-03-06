#pragma once

#include "scene.h"
#include "gui.h"

namespace sprint::editor::gui {

template<class T>
void DrawComponentContent(meta::Reference ref) {
    static const meta::Type type = meta::GetType<T>();
    static const std::vector<meta::Field>& fields = type.GetFields();
    for (const auto & field : fields) {
        gui::DrawField(ref, field);
    }
}

template<>
void DrawComponentContent<TransformComponent>(meta::Reference ref);

template<class T>
bool DrawComponent(Scene* scene, ecs::entity_t selected) {
    static const meta::Type type = meta::GetType<T>();

    gui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 4.0f);
    bool node_open = gui::TreeNodeEx(type.FullName().c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected, "%s", type.Name().c_str());

    if (node_open) {
        gui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        meta::Reference ref(scene->get<T>(selected));
        DrawComponentContent<T>(ref);
        gui::PopStyleVar();

        gui::TreePop();
    }

    gui::PopStyleVar();
    return node_open;
}

}


