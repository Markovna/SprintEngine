#pragma once

#include "window_event.h"

#include <queue>
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
    static std::unique_ptr<Window> Create(Vec2Int size);

    Window(size_t width, size_t height);
    ~Window();

    void Update();
    bool PollEvent(Event& event);
    void SetMouseCursor(Cursor::Type cursor_type);

    [[nodiscard]] size_t get_width() const { return width_; }
    [[nodiscard]] size_t get_height() const { return height_; }
    [[nodiscard]] Vec2Int get_resolution() const { return resolution_; }
    [[nodiscard]] vec2 get_scale() const { return content_scale_; }
    [[nodiscard]] GLFWwindow* get_handle() const { return window_; }

private:
    void PushEvent(Event event);

private:
    size_t width_;
    size_t height_;
    vec2 content_scale_;
    Vec2Int resolution_;
    GLFWwindow* window_;
    std::queue<Event> events_;
};


}