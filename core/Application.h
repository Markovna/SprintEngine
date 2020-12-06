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
    static event<MouseDownEvent&> OnMouseDown;
    static event<MouseUpEvent&> OnMouseUp;
    static event<MouseMoveEvent&> OnMouseMove;
    static event<ScrollEvent&> OnScroll;
    static event<KeyPressEvent&> OnKeyPress;
    static event<KeyReleaseEvent&> OnKeyRelease;
    static event<TextEvent&> OnTextInput;

public:
    Application();
    ~Application();
    int Run();
private:
    bool RunOneFrame();
    void OnEvent(WindowEvent &event);

    template<class T> void OnEvent(T&) {
        assert(false);
    }

private:
    std::unique_ptr<Window> window_;
    std::unique_ptr<ImGuiRenderer> imgui_renderer_;
    Clock clock_;
};


}