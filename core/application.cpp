#include "gfx.h"
#include "application.h"
#include "window.h"
#include "engine.h"
#include "input_events.h"
#include "config.h"

namespace sprint {

Application::Application() {
    SPRINT_PROFILE_FUNCTION();

    window_ = Window::Create(config::window_size);
    gfx::Init({ window_->get_handle(), window_->get_resolution() });
    engine_ = Engine::Create(*window_);

    input_events::OnClose.connect(this, &Application::OnClose);
    input_events::OnResize.connect(this, &Application::OnResize);
}

int Application::Run() {
    while (running_) {
        window_->Update();
        WindowEvent event;
        while (running_ && window_->PollEvent(event)) {
            input_events::OnEvent(event);
        }

        engine_->Update();
        gfx::Frame();
    }
    return 0;
}

Application::~Application() {
    input_events::OnClose.disconnect(this, &Application::OnClose);
    input_events::OnResize.disconnect(this, &Application::OnResize);

    engine_.reset();
    gfx::Shutdown();
    window_.reset();
}

void Application::OnClose(CloseEvent&) {
    running_ = false;
}

void Application::OnResize(ResizeEvent &) {
    gfx::SetResolution(window_->get_resolution());
}

}