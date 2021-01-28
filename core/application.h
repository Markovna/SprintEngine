#pragma once

#include "window_event.h"
#include "window.h"
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
    void OnClose(CloseEvent&);
    void OnResize(ResizeEvent&);

private:
    std::unique_ptr<Window> window_;
    std::unique_ptr<Engine> engine_;
    bool running_ = true;
};


}