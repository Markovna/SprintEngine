#pragma once

#include <memory>

namespace sprint {

class Engine;

namespace editor {

class EditorGui;

class PropertiesEditorGui {
public:
    static std::unique_ptr<PropertiesEditorGui> Create(EditorGui&, Engine&);

    PropertiesEditorGui(EditorGui& editor, Engine& engine)
        : editor_(editor), engine_(engine) {}

    ~PropertiesEditorGui() = default;

    void OnGui();
private:
    EditorGui &editor_;
    Engine &engine_;
};

}

}