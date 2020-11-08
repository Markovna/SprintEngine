#pragma once

#include "WindowEvent.h"
#include "Window.h"

#include "chrono"


namespace sprint {


typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds> TimeSpan;

class Application {
public:
    Application();
    int Run();
private:
    void UpdateTime();
    bool RunOneFrame();
private:
    std::unique_ptr<Window> window_;

    float delta_time_ = 0;
    TimeSpan last_update_time_;
};


}