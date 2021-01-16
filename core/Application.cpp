#include <gfx.h>
#include "Application.h"
#include "Window.h"
#include "Log.h"
#include "app_time.h"

namespace sprint {

namespace time {

Clock clock_;
TimeSpan delta_;

void Restart() { delta_ = clock_.Restart(); }

TimeSpan delta() { return delta_; }

}

// TODO: put all this stuff to some config
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 700;

event<MouseDownEvent&> Application::OnMouseDown;
event<MouseUpEvent&> Application::OnMouseUp;
event<MouseMoveEvent&> Application::OnMouseMove;
event<ScrollEvent&> Application::OnScroll;
event<KeyPressEvent&> Application::OnKeyPress;
event<KeyReleaseEvent&> Application::OnKeyRelease;
event<TextEvent&> Application::OnTextInput;

Application::Application() {

    window_ = std::make_unique<Window>(SCREEN_WIDTH, SCREEN_HEIGHT);
    gfx::Init( { window_->get_handle() } );

    imgui_renderer_ = std::make_unique<ImGuiRenderer>();
    engine_ = std::make_unique<Engine>(*window_);
}

int Application::Run() {
    while (running_) { time::Restart(); Frame(); }
    return 0;
}

template<> void Application::OnEvent(MouseDownEvent& e) { OnMouseDown(e); }
template<> void Application::OnEvent(MouseUpEvent& e) { OnMouseUp(e); }
template<> void Application::OnEvent(MouseMoveEvent& e) { OnMouseMove(e); }
template<> void Application::OnEvent(KeyPressEvent& e) { OnKeyPress(e); }
template<> void Application::OnEvent(KeyReleaseEvent& e) { OnKeyRelease(e); }
template<> void Application::OnEvent(ScrollEvent& e) { OnScroll(e); }
template<> void Application::OnEvent(TextEvent& e) { OnTextInput(e); }
template<> void Application::OnEvent(CloseEvent& e) { running_ = false; }

void Application::OnEvent(WindowEvent& event) {
    mpark::visit([this](auto& e) { OnEvent(e); }, event);
}

void Application::Frame() {


    window_->Update();

    WindowEvent event;
    while (running_ && window_->PollEvent(event)) { OnEvent(event); }

    imgui_renderer_->BeginFrame(window_.get());

    engine_->Update();

    imgui_renderer_->EndFrame();

    gfx::Frame();
}

Application::~Application() {
    window_.reset();
    imgui_renderer_.reset();
    engine_.reset();

    gfx::Shutdown();
}

}