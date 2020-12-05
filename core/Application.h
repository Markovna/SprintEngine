#pragma once

#include <clock.h>
#include "WindowEvent.h"
#include "Window.h"
#include "signals/event.h"

#include "../editor/imgui_renderer.h"

namespace sprint {

class Application {
public:
    // TODO: move it to InputSystem
    static event<MouseEvent&> OnMouseDown;
    static event<MouseEvent&> OnMouseUp;
    static event<MouseMoveEvent&> OnMouseMove;
    static event<ScrollEvent&> OnScroll;
    static event<KeyEvent&> OnKeyPress;
    static event<KeyEvent&> OnKeyRelease;
    static event<TextEvent&> OnTextInput;

public:
    Application();
    ~Application();
    int Run();
private:
    bool RunOneFrame();
    void OnEvent(WindowEvent &event);
private:
    std::unique_ptr<Window> window_;
    std::unique_ptr<ImGuiRenderer> imgui_renderer_;
    Clock clock_;
};


}