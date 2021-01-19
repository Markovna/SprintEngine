#include <gfx.h>
#include "Application.h"
#include "Window.h"
#include "Log.h"
#include "app_time.h"
#include "input_events.h"

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

Application::Application() {
    window_ = std::make_unique<Window>(SCREEN_WIDTH, SCREEN_HEIGHT);
    gfx::Init({ window_->get_handle(), window_->get_resolution() });

    imgui_renderer_ = std::make_unique<ImGuiRenderer>();
    engine_ = std::make_unique<Engine>(*window_);

    input_events::OnClose.connect(this, &Application::OnClose);
    input_events::OnResize.connect(this, &Application::OnResize);
}

int Application::Run() {
    while (running_) { Frame(); }
    return 0;
}

void Application::Frame() {

    time::Restart();

    window_->Update();

    WindowEvent event;
    while (running_ && window_->PollEvent(event)) { input_events::OnEvent(event); }

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

    input_events::OnClose.disconnect(this, &Application::OnClose);
    input_events::OnResize.disconnect(this, &Application::OnResize);
}

void Application::OnClose(CloseEvent&) {
    running_ = false;
}

void Application::OnResize(ResizeEvent &) {
    gfx::SetResolution(window_->get_resolution());
}

}