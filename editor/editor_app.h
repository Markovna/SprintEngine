#pragma once

#include "window.h"
#include "engine.h"
#include "imgui_renderer.h"
#include "editor_gui.h"

#include <memory>

namespace sprint::editor {

class Application {
public:
    Application();
    ~Application();
    int Run();
private:
    void OnClose(CloseEvent&);
    void OnResize(ResizeEvent&);
private:
    std::unique_ptr<Window> window_;
    std::unique_ptr<Engine> engine_;
    std::unique_ptr<ImGuiRenderer> imgui_renderer_;
    std::unique_ptr<EditorGui> editor_;
    bool running_ = true;
};

};

