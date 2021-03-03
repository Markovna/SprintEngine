#pragma once

namespace sprint {

class Engine;

namespace editor {

class EditorGui;

class SceneViewGui {
public:
    static std::unique_ptr<SceneViewGui> Create(EditorGui&, Engine&);

    SceneViewGui(EditorGui& editor, Engine& engine)
        : editor_(editor), engine_(engine) {}

    ~SceneViewGui() = default;

    void OnGui();
private:
    EditorGui &editor_;
    Engine &engine_;
};

}

}