#pragma once

#include <variant>
#include "core/scene.h"

namespace sprint {

class Engine;

namespace editor {

class EditorGui;

struct SetParentCommand {
    Entity entity;
    Entity parent;
    Entity next;

    void Execute(Engine &engine) const;
};

struct CreateEntityCommand {
    Entity parent;

    void Execute(Engine &engine) const;
};

struct DestroyEntityCommand {
    Entity entity;

    void Execute(Engine &engine) const;
};

using EditorGuiCommand = std::variant<SetParentCommand, CreateEntityCommand, DestroyEntityCommand>;

class SceneGraphEditorGui {
public:
    static std::unique_ptr<SceneGraphEditorGui> Create(EditorGui &, Engine &);

    SceneGraphEditorGui(EditorGui &editor, Engine &engine) : editor_(editor), engine_(engine) {}
    ~SceneGraphEditorGui() = default;

    void OnGui();

private:
    void DrawSceneGraph();

private:
    EditorGui &editor_;
    Engine &engine_;
    std::vector<EditorGuiCommand> commands_;
};

}

}