#pragma once

#include "engine.h"

namespace sprint {

class Renderer;

}

namespace sprint::editor {

class SceneGraphEditorGui;
class PropertiesEditorGui;
class SceneViewGui;

class EditorGui {
public:
    static std::unique_ptr<EditorGui> Create(Window&, Engine&);

    EditorGui(Window&, Engine&);
    ~EditorGui();

    [[nodiscard]] ecs::entity_t Selected() const { return selected_; }
    void Select(ecs::entity_t entity) { selected_ = entity; }

    void OnGui();
private:
    Window& window_;
    Engine& engine_;
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<SceneGraphEditorGui> scene_graph_gui_;
    std::unique_ptr<PropertiesEditorGui> properties_gui_;
    std::unique_ptr<SceneViewGui> scene_view_gui_;
    ecs::entity_t selected_ = ecs::null;
};

}
