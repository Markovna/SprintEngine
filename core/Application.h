#pragma once

#include "Event.h"
#include "Window.h"

#include "chrono"

using TimeSpan = std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>;

class Application {
public:
    Application();
    int Run();
private:
    void OnEvent(Event& event);
    void OnClose();
    void UpdateTime();
private:
    bool m_Running = false;
    std::unique_ptr<Window> m_Window;

    float m_DeltaTime;
    TimeSpan m_LastUpdateTime;
};


