#pragma once

namespace sprint {

class Engine;
class RenderTexture;
class Renderer;

namespace editor {

class EditorGui;

class SceneViewGui {
public:
    static std::unique_ptr<SceneViewGui> Create(EditorGui&, Engine&);

    SceneViewGui(EditorGui& editor, Engine& engine);

    ~SceneViewGui();

    void OnGui();
private:
    void MoveCamera(const vec2& delta);
    void ZoomCamera(float delta);
    void RotateCamera(const vec2& delta);

private:
    EditorGui &editor_;
    Engine &engine_;
    std::unique_ptr<Renderer> renderer_;
    uint32_t camera_;
    std::unique_ptr<RenderTexture> render_texture_;
};

}

}