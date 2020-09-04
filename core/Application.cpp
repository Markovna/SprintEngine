#include "Application.h"
#include "Window.h"
#include "Log.h"

namespace Sprint {

// TODO: put all this stuff to some config
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

Application::Application() {
    Log::Init();

    m_Window = std::make_unique<Window>(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void Application::UpdateTime() {
    using namespace std::chrono;
    TimeSpan now = steady_clock::now();
    m_DeltaTime = duration_cast<microseconds>(now - m_LastUpdateTime).count() / 1000000.0f;
    m_LastUpdateTime = now;
}

int Application::Run() {
    while (RunOneFrame());
    return 0;
}

void OnMouseDown(MouseEvent& event) {}
void OnMouseUp(MouseEvent& event) {}
void OnMouseMove(MouseMoveEvent& event) {}
void OnKeyPress(KeyEvent& event) {}
void OnKeyRelease(KeyEvent& event) {}

void OnEvent(WindowEvent& event) {
    switch (event.GetType()) {
        case WindowEvent::Type::MOUSE_DOWN:  OnMouseDown(event.Mouse); break;
        case WindowEvent::Type::MOUSE_UP:    OnMouseUp(event.Mouse); break;
        case WindowEvent::Type::MOUSE_MOVE:  OnMouseMove(event.MouseMove); break;
        case WindowEvent::Type::KEY_PRESS:   OnKeyPress(event.Key); break;
        case WindowEvent::Type::KEY_RELEASE: OnKeyRelease(event.Key); break;
        default: break;
    }
}


bool Application::RunOneFrame() {
    UpdateTime();

    WindowEvent event;
    while (m_Window->PollEvent(event)) {
        if (event.GetType() == WindowEvent::Type::CLOSE)
            return false;

        OnEvent(event);
    }

    m_Window->OnUpdate();
    return true;
}
}