#include "core/resources.h"
#include "core/input_events.h"
#include "editor_app.h"

namespace sprint::editor {

Application::Application(std::filesystem::path project_path) {
    SPRINT_PROFILE_FUNCTION();

    std::filesystem::current_path(project_path);

    window_ = Window::Create({1024, 640});
    gfx::Init(gfx::Config {window_->get_handle(), window_->get_resolution()});
    imgui_renderer_ = ImGuiRenderer::Create();
    engine_ = Engine::Create();
    editor_ = EditorGui::Create(*engine_);

    input_events::OnClose.connect(this, &Application::OnClose);
    input_events::OnResize.connect(this, &Application::OnResize);
}

Application::~Application() {
    SPRINT_PROFILE_FUNCTION();

    input_events::OnClose.disconnect(this, &Application::OnClose);
    input_events::OnResize.disconnect(this, &Application::OnResize);

    editor_.reset();
    engine_.reset();
    imgui_renderer_.reset();
    gfx::Shutdown();
    window_.reset();
}

int Application::Run() {

    while (running_) {

        window_->Update();
        WindowEvent event;
        while (running_ && window_->PollEvent(event)) { input_events::OnEvent(event); }

        imgui_renderer_->BeginFrame(window_.get());

        engine_->Update();

        editor_->OnGui();

        imgui_renderer_->EndFrame();

        gfx::Frame();
    }

    return 0;
}

void Application::OnClose(CloseEvent&) {
    running_ = false;
}

void Application::OnResize(ResizeEvent &) {
    gfx::SetResolution(window_->get_resolution());
}

};
