#pragma once

#include "WindowEvent.h"
#include "Window.h"

#include "chrono"


namespace Sprint {


typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> TimeSpan;

class Application {
public:
    Application();
    int Run();
private:
    void UpdateTime();
    bool RunOneFrame();
private:
    std::unique_ptr<Window> m_Window;

    float m_DeltaTime = 0;
    TimeSpan m_LastUpdateTime;
};


}