#include "input_events.h"
#include "editor_app.h"

#include <filesystem>

namespace sprint::editor {

Application::Application(Config config) {
    SPRINT_PROFILE_FUNCTION();

    std::filesystem::path path(config.project_path);
    std::filesystem::current_path(path);

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


Application::Config Application::Config::Parse(int argc, char *argv[]) {
    Application::Config config;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i-1], "-p") == 0) {
            config.project_path = argv[i];
        }
    }
    return config;
}

};
