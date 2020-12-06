#include <GL.h>
#include <gfx.h>
#include "Application.h"
#include "Window.h"
#include "Log.h"

namespace sprint {

// TODO: put all this stuff to some config
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

event<MouseDownEvent&> Application::OnMouseDown;
event<MouseUpEvent&> Application::OnMouseUp;
event<MouseMoveEvent&> Application::OnMouseMove;
event<ScrollEvent&> Application::OnScroll;
event<KeyPressEvent&> Application::OnKeyPress;
event<KeyReleaseEvent&> Application::OnKeyRelease;
event<TextEvent&> Application::OnTextInput;

Application::Application() {

    window_ = std::make_unique<Window>(SCREEN_WIDTH, SCREEN_HEIGHT);

    gfx::Init(gfx::Config { .window_handle = window_->get_handle() } );

    imgui_renderer_ = std::make_unique<ImGuiRenderer>();
}

int Application::Run() {
    while (RunOneFrame());
    return 0;
}

template<> void Application::OnEvent(MouseDownEvent& e) { OnMouseDown(e); }
template<> void Application::OnEvent(MouseUpEvent& e) { OnMouseUp(e); }
template<> void Application::OnEvent(MouseMoveEvent& e) { OnMouseMove(e); }
template<> void Application::OnEvent(KeyPressEvent& e) { OnKeyPress(e); }
template<> void Application::OnEvent(KeyReleaseEvent& e) { OnKeyRelease(e); }
template<> void Application::OnEvent(ScrollEvent& e) { OnScroll(e); }
template<> void Application::OnEvent(TextEvent& e) { OnTextInput(e); }

void Application::OnEvent(WindowEvent& event) {
    mpark::visit([this](auto& e) { OnEvent(e); }, event);
}

bool Application::RunOneFrame() {
    TimeSpan delta = clock_.Restart();

    WindowEvent event;
    while (window_->PollEvent(event)) {
        if (event.get_type() == EventType::CLOSE) {
            return false;
        }

        OnEvent(event);
    }

    float fps = 1.0f / delta.AsSeconds();

    imgui_renderer_->BeginFrame(window_.get(), delta.AsSeconds());

    window_->OnUpdate();

    {
        ImGui::Begin("Console");

//        ImGui::Text("This is some useful text.");

//        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
//        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color



        static char buf[1024];
        buf[0] = '>'; buf[1] = ' ';

//        ImGui::Text(">");
//        ImGui::SameLine();
        ImGui::InputTextMultiline("", buf, 1024);
//        ImGui::SameLine();
//        if (ImGui::Button("test")) {
//
//        }
        if (ImGui::Button("test2")) {

        }

        ImGui::Text("%s", log::Format("fps={0:03.3f}", fps).c_str());

        ImGui::End();


//        ImGui::Begin("Window");
//        ImGui::Text("Text");
//        ImGui::End();
    }

    imgui_renderer_->EndFrame();

    gfx::Frame();

    return true;
}

Application::~Application() {

    window_.reset();
    imgui_renderer_.reset();

    gfx::Shutdown();

}

}