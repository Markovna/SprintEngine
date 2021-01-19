#pragma once

#include "WindowEvent.h"
#include "Window.h"
#include "signals/event.h"

#include "../editor/imgui_renderer.h"
#include "engine.h"

namespace sprint {

class Application {
public:
    Application();
    ~Application();
    int Run();
private:
    void Frame();
    void OnClose(CloseEvent&);

private:
    std::unique_ptr<Window> window_;
    std::unique_ptr<ImGuiRenderer> imgui_renderer_;
    std::unique_ptr<Engine> engine_;
    bool running_ = true;
};


}