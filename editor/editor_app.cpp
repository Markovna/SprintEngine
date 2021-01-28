#include <input_events.h>
#include "editor_app.h"
#include "../debug/profiler.h"

namespace sprint::editor {

//TODO
static std::unique_ptr<Texture> render_tex;
static gfx::texture_handle depth_tex;
static gfx::framebuf_handle frame_buffer;
static void TestGui(float delta) {
    ImGui::Text("%s", log::Format("fps={0:.5f}", 1.0f / delta).c_str());
    static bool show = true;
    ImGui::ShowDemoWindow(&show);
}

Application::Application() {
    SPRINT_PROFILE_FUNCTION();

    window_ = Window::Create({1024, 640});
    gfx::Init(gfx::Config {window_->get_handle(), window_->get_resolution()});
    imgui_renderer_ = ImGuiRenderer::Create();
    engine_ = Engine::Create(*window_);
    editor_ = EditorGUI::Create();

    input_events::OnClose.connect(this, &Application::OnClose);
    input_events::OnResize.connect(this, &Application::OnResize);

    // TODO
    {
        render_tex = std::make_unique<Texture>(Texture{
            gfx::MemoryPtr{}, (uint32_t) window_->get_resolution().x, (uint32_t) window_->get_resolution().y,
            gfx::TextureFormat::RGBA8, gfx::TextureWrap{}, gfx::TextureFilter{}, gfx::TextureFlags::None});
        depth_tex = gfx::CreateTexture((uint32_t) window_->get_resolution().x,
                                       (uint32_t) window_->get_resolution().y,
                                       gfx::TextureFormat::D24S8,
                                       {},
                                       {},
                                       gfx::TextureFlags::RenderTarget,
                                       {});
        frame_buffer = gfx::CreateFrameBuffer({render_tex->get_handle(), depth_tex});

        engine_->SetOutput(frame_buffer);
    }
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

        // TODO
        {
            TestGui(engine_->get_delta().AsSeconds());
            float width = ImGui::GetWindowWidth();
            float height = render_tex->get_height() * (width / render_tex->get_width());
            ImGui::Image((ImTextureID)(intptr_t)render_tex->get_handle().id, {width, height}, {0,1}, {1, 0});
        }

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
