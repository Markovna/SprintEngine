#pragma once

#include "engine.h"

namespace sprint {

class Renderer;

}

namespace sprint::editor {

class SceneGraphEditorGui;
class PropertiesEditorGui;
class SceneViewGui;
class GameViewGui;

class EditorGui {
public:
    static std::unique_ptr<EditorGui> Create(Engine&);

    explicit EditorGui(Engine&);
    ~EditorGui();

    [[nodiscard]] ecs::entity_t Selected() const { return selected_; }
    void Select(ecs::entity_t entity) { selected_ = entity; }

    void OnGui();
private:
    Engine& engine_;
    std::unique_ptr<GameViewGui> game_view_gui;
    std::unique_ptr<SceneGraphEditorGui> scene_graph_gui_;
    std::unique_ptr<PropertiesEditorGui> properties_gui_;
    std::unique_ptr<SceneViewGui> scene_view_gui_;
    ecs::entity_t selected_ = ecs::null;
};

}
