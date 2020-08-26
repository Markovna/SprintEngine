#include "Application.h"
#include "Window.h"
#include "Log.h"

// TODO: put all this stuff to some config
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

Application::Application() {
    Log::Init();

    m_Window = std::make_unique<Window>(SCREEN_WIDTH, SCREEN_HEIGHT);
    m_Window->SetEventCallback([this](Event &e) { OnEvent(e); });
    m_Window->SetCloseCallback([this]() { OnClose(); });
}

void Application::UpdateTime() {
    using namespace std::chrono;
    time_point now = steady_clock::now();
    m_DeltaTime = duration_cast<microseconds>(now - m_LastUpdateTime).count() / 1000000.0f;
    m_LastUpdateTime = now;
}

int Application::Run() {
    m_Running = true;

    while (m_Running) {

        UpdateTime();

        m_Window->OnUpdate();
    }

    return 0;
}

void Application::OnEvent(Event &event) {
}

void Application::OnClose() {
    m_Running = false;
}
