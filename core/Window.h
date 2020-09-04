#pragma once

#include "WindowEvent.h"

#include <queue>
#include <functional>
#include <GLFW/glfw3.h>

namespace Sprint {

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
    int m_Width, m_Height;
    GLFWwindow* m_Window;
    std::queue<Event> m_Events;
};


}