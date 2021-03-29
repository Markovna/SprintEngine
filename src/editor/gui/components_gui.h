#pragma once

#include "core/scene.h"
#include "gui.h"

namespace sprint::editor::gui {

template<class T>
bool DrawComponentContent(World* scene, Entity entity, meta::Reference ref) {
    static const meta::Type type = meta::GetType<T>();
    static const std::vector<meta::Field>& fields = type.GetFields();
    bool changed = false;
    for (const auto & field : fields) {
        changed |= gui::DrawField(ref, field);
    }
    return changed;
}

template<>
bool DrawComponentContent<TransformComponent>(World* world, Entity entity, meta::Reference ref);

template<class T>
bool DrawComponent(World* world, Entity selected) {
    static const meta::Type type = meta::GetType<T>();

    gui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 4.0f);
    bool node_open = gui::TreeNodeEx(type.FullName().c_str(), ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_Selected, "%s", type.Name().c_str());

    if (node_open) {
        gui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(2, 2));
        meta::Reference ref(world->Get<T>(selected));

        DrawComponentContent<T>(world, selected, ref);

        gui::PopStyleVar();

        gui::TreePop();
    }

    gui::PopStyleVar();
    return node_open;
}

}


