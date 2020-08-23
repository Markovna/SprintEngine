#pragma once

#include "Event.h"

#include <functional>
#include <GLFW/glfw3.h>

class Window {
public:
    using EventCallback = std::function<void(Event&)>;
    using CloseCallback = std::function<void()>;

    Window(int width, int height);
    ~Window();

    void OnUpdate();

    void SetEventCallback(const EventCallback& callback) {
        m_EventCallback = callback;
    }

    void SetCloseCallback(const CloseCallback& callback) {
        m_CloseCallback = callback;
    }

private:
    int m_Width, m_Height;
    GLFWwindow* m_Window;
    EventCallback m_EventCallback;
    CloseCallback m_CloseCallback;
};


