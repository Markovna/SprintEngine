#pragma once

#include "core/window.h"
#include "core/engine.h"
#include "editor/gui/imgui_renderer.h"
#include "editor/gui/editor_gui.h"

#include <memory>
#include <filesystem>

namespace sprint::editor {

class Application {

public:
    explicit Application(std::filesystem::path);
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

