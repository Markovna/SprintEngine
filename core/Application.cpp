#include <GL.h>
#include <gfx.h>
#include "Application.h"
#include "Window.h"
#include "Log.h"

namespace sprint {

// TODO: put all this stuff to some config
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

event<MouseEvent&> Application::OnMouseDown;
event<MouseEvent&> Application::OnMouseUp;
event<MouseMoveEvent&> Application::OnMouseMove;
event<KeyEvent&> Application::OnKeyPress;
event<KeyEvent&> Application::OnKeyRelease;

Application::Application() {

    window_ = std::make_unique<Window>(SCREEN_WIDTH, SCREEN_HEIGHT);

//    gl::Init(gl::Config { gl::GraphicContext(window_->get_handle()) });
    gfx::Init(gfx::Config { .window_handle = window_->get_handle() } );

    imgui_renderer_ = std::make_unique<ImGuiRenderer>();
}

int Application::Run() {
    while (RunOneFrame());
    return 0;
}

void Application::OnEvent(WindowEvent& event) {
    switch (event.get_type()) {
        case WindowEvent::MOUSE_DOWN:
            OnMouseDown(event.Mouse);
            break;
        case WindowEvent::MOUSE_UP:
            OnMouseUp(event.Mouse);
            break;
        case WindowEvent::MOUSE_MOVE:
            OnMouseMove(event.MouseMove);
            break;
        case WindowEvent::KEY_PRESS:
            OnKeyPress(event.Key);
            break;
        case WindowEvent::KEY_RELEASE:
            OnKeyRelease(event.Key);
            break;
        default:
            break;
    }
}

bool Application::RunOneFrame() {
    TimeSpan delta = clock_.Restart();

    WindowEvent event;
    while (window_->PollEvent(event)) {
        if (event.get_type() == WindowEvent::CLOSE) {
            return false;
        }

        OnEvent(event);
    }

    float fps = 1.0f / delta.AsSeconds();
//    log::core ::Info("====== fps {0}", fps);

    imgui_renderer_->BeginFrame(window_.get(), delta.AsSeconds());

    window_->OnUpdate();

    imgui_renderer_->EndFrame();

    return true;
}

Application::~Application() {
//    gl::Shutdown();
    gfx::Shutdown();
}

}