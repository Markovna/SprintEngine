#pragma once

#include "engine.h"

namespace sprint::editor {

struct SetParentCommand {
    ecs::entity_t entity;
    ecs::entity_t parent;
    ecs::entity_t next;

    void Execute(Engine& engine) const;
};

struct CreateEntityCommand {
    ecs::entity_t parent;

    void Execute(Engine& engine) const;
};

using EditorGuiCommand = std::variant<SetParentCommand, CreateEntityCommand>;

class SceneGraphEditorGui {
public:
    static std::unique_ptr<SceneGraphEditorGui> Create(Engine&);

    explicit SceneGraphEditorGui(Engine& e) : engine_(e) {};
    ~SceneGraphEditorGui() = default;

    void OnGui();

private:
    Engine& engine_;
    ecs::entity_t selected_ = ecs::null;
    std::vector<EditorGuiCommand> commands_;
};

}

