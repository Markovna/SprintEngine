#pragma once

#include "WindowEvent.h"

#include <queue>
#include <functional>
#include <GLFW/glfw3.h>

namespace sprint {

class Window {
public:
    struct Cursor {
        enum Type {
            None = -1,
            Arrow,
            Text_Input,
            Resize_Vertical,
            Resize_Horizontal,
            Resize_All,
            Resize_Bottom_Left_Top_Right,
            Resize_Top_Left_Bottom_Right,
            Hand,
            Not_Allowed,

            Count
        };
    };

private:
    using Event = WindowEvent;
public:

    Window(size_t width, size_t height);
    ~Window();

    void OnUpdate();
    bool PollEvent(Event& event);
    void SetMouseCursor(Cursor::Type cursor_type);

    size_t get_width() const { return width_; }
    size_t get_height() const { return height_; }
    GLFWwindow* get_handle() const { return window_; }

private:
    void PushEvent(Event event);

private:
    size_t width_, height_;
    GLFWwindow* window_;
    std::queue<Event> events_;
};


}