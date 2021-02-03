#pragma once

#include "window.h"
#include "engine.h"
#include "scene_graph_gui.h"

namespace sprint::editor {

class EditorGui {
public:
    static std::unique_ptr<EditorGui> Create(Window&, Engine&);

    EditorGui(Window&, Engine&);
    ~EditorGui();

    void OnGui();
private:
    Window& window_;
    Engine& engine_;
    std::unique_ptr<SceneGraphEditorGui> scene_graph_gui_;
};

}
