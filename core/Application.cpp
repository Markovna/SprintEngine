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
event<ScrollEvent&> Application::OnScroll;
event<KeyEvent&> Application::OnKeyPress;
event<KeyEvent&> Application::OnKeyRelease;
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

void Application::OnEvent(WindowEvent& event) {
    switch (event.get_type()) {
        case WindowEvent::MOUSE_DOWN:
            OnMouseDown(event.mouse_event);
            break;
        case WindowEvent::MOUSE_UP:
            OnMouseUp(event.mouse_event);
            break;
        case WindowEvent::MOUSE_MOVE:
            OnMouseMove(event.mouse_move_event);
            break;
        case WindowEvent::SCROLL:
            OnScroll(event.scroll_event);
            break;
        case WindowEvent::KEY_PRESS:
            OnKeyPress(event.key_event);
            break;
        case WindowEvent::KEY_RELEASE:
            OnKeyRelease(event.key_event);
            break;
        case WindowEvent::TEXT:
            OnTextInput(event.text_event);
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

    imgui_renderer_->BeginFrame(window_.get(), delta.AsSeconds());

    window_->OnUpdate();

    {
        ImGui::Begin("Console");

//        ImGui::Text("This is some useful text.");

//        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
//        ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

        if (ImGui::Button("test")) {

        }

        static char buf[1024];

        ImGui::Text(">");
        ImGui::SameLine();
        ImGui::InputTextMultiline("", buf, 1024);
//        ImGui::SameLine();
        ImGui::Text("%s", log::Format("fps={0}", fps).c_str());

        ImGui::End();

        ImGui::Begin("Window");
        ImGui::Text("Text");
        ImGui::End();

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