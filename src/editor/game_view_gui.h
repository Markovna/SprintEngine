#pragma once

#include "renderer/gfx.h"
#include "math/vector.h"

namespace sprint {

class Engine;
class Renderer;
class RenderTexture;

}

namespace sprint::editor {

class EditorGui;

class GameViewGui {
public:
    static std::unique_ptr<GameViewGui> Create(EditorGui&, Engine&);

    GameViewGui(EditorGui& editor, Engine& engine);

    ~GameViewGui();

    void OnGui();
private:
    EditorGui &editor_;
    Engine &engine_;
    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<RenderTexture> render_texture_;
};

}