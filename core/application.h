#pragma once

#include "window_event.h"

namespace sprint {

class Window;
class Engine;

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