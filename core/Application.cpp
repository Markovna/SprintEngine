#include "Application.h"
#include "Window.h"
#include "Log.h"

namespace sprint {

// TODO: put all this stuff to some config
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

Application::Application() {
    Log::Init();

    window_ = std::make_unique<Window>(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Application::UpdateTime() {
    using namespace std::chrono;
    TimeSpan now = steady_clock::now();
    delta_time_ = duration_cast<microseconds>(now - last_update_time_).count() / 1000000.0f;
    last_update_time_ = now;
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
    UpdateTime();

    WindowEvent event;
    while (window_->PollEvent(event)) {
        if (event.get_type() == WindowEvent::CLOSE) {
            return false;
        }

        OnEvent(event);
    }

    window_->OnUpdate();
    return true;
}
}