#pragma once

#include "WindowEvent.h"

#include <queue>
#include <functional>
#include <GLFW/glfw3.h>

namespace sprint {

class Window {
private:
    using Event = WindowEvent;
public:

    Window(int width, int height);
    ~Window();

    void OnUpdate();
    bool PollEvent(Event& event);

private:
    void PushEvent(Event event);

private:
    int width_, height_;
    GLFWwindow* window_;
    std::queue<Event> events_;
};


}