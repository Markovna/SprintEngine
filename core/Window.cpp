#include "Log.h"

#include <cmath>

#include "Shader.h"
#include "Window.h"
#include "Vector.h"
#include "Matrix.h"
#include "Texture.h"
#include "color.h"
#include "gfx.h"
#include "Application.h"

namespace sprint {

static void GLFWErrorCallback(int error_code, const char* description) {
    log::core::Error("GLFW error ({1}): {0}", description, error_code);
}

Window::Window(size_t width, size_t height) : width_(width), height_(height) {
    int status = glfwInit();
    assert(status); // TODO: assert macro
    glfwSetErrorCallback(&GLFWErrorCallback);

    // TODO: move gl version to config
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window_ = glfwCreateWindow(width_, height_, "SprintEngine", NULL, NULL);
    assert(window_); // TODO: assert macro

    glfwMakeContextCurrent(window_);

//    float x, y; glfwGetWindowContentScale(window_, &x, &y);
    int buffer_w, buffer_h; glfwGetFramebufferSize(window_, &buffer_w, &buffer_h);
    resolution_ = { buffer_w, buffer_h };

    glfwSetWindowUserPointer(window_, this);

    // set input callbacks
    glfwSetKeyCallback(window_, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
        Window* window = (Window*) glfwGetWindowUserPointer(w);
        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            window->PushEvent(
                KeyPressEvent {
                    (KeyCode)key,
                    bool(mods & GLFW_MOD_CONTROL),
                    bool(mods & GLFW_MOD_SHIFT),
                    bool(mods & GLFW_MOD_ALT),
                    bool(mods & GLFW_MOD_SUPER),
                    action == GLFW_REPEAT
                    });
        }
        else if (action == GLFW_RELEASE) {
            window->PushEvent(
                KeyReleaseEvent {
                    KeyCode(key),
                    bool(mods & GLFW_MOD_CONTROL),
                    bool(mods & GLFW_MOD_SHIFT),
                    bool(mods & GLFW_MOD_ALT),
                    bool(mods & GLFW_MOD_SUPER)
                    });
        }
    });

    glfwSetMouseButtonCallback(window_, [](GLFWwindow *w, int button, int action, int mods) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        if (action == GLFW_PRESS) {
            window->PushEvent(MouseDownEvent{(MouseCode)button});
        }
        else if (action == GLFW_RELEASE) {
            window->PushEvent(MouseUpEvent{(MouseCode)button});
        }
    });

    glfwSetScrollCallback(window_, [](GLFWwindow* w, double xoffset, double yoffset){
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        window->PushEvent(ScrollEvent { Vec2(float(xoffset), float(yoffset)) });
    });

    glfwSetCursorPosCallback(window_, [](GLFWwindow *w, double xpos, double ypos) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        window->PushEvent(MouseMoveEvent { Vec2(float(xpos), float(ypos)) });
    });

    glfwSetCharCallback(window_, [](GLFWwindow* w, unsigned int codepoint) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        window->PushEvent(TextEvent { codepoint } );
    });

    glfwSetWindowCloseCallback(window_, [](GLFWwindow *w) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        window->PushEvent(CloseEvent());
    });

    glfwSetWindowSizeCallback(window_, [](GLFWwindow *w, int width, int height) {
        Window* window = (Window*) glfwGetWindowUserPointer(w);
        window->width_ = width;
        window->height_ = height;

        int buffer_w, buffer_h; glfwGetFramebufferSize(w, &buffer_w, &buffer_h);
        window->resolution_ = { buffer_w, buffer_h };

        window->PushEvent(ResizeEvent{});
    });

//    glfwSetWindowRefreshCallback(window_, [](GLFWwindow *w) {
//    });
}

Window::~Window() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void Window::Update() {
    glfwPollEvents();
}

bool Window::PollEvent(Event& event) {
    if (events_.empty())
        return false;

    event = events_.front();
    events_.pop();
    return true;
}

void Window::PushEvent(Event event) {
    events_.emplace(std::move(event));
}

GLFWcursor* ToGlfwCursor(Window::Cursor::Type cursor_type) {
    static std::array<GLFWcursor*, Window::Cursor::Count> mouse_cursors = [](){
        std::array<GLFWcursor*, Window::Cursor::Count> cursors;
        cursors[Window::Cursor::Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        cursors[Window::Cursor::Text_Input] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        cursors[Window::Cursor::Resize_Vertical] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
        cursors[Window::Cursor::Resize_Horizontal] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        cursors[Window::Cursor::Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
#if GLFW_HAS_NEW_CURSORS
        cursors[Window::Cursor::Resize_All] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
        cursors[Window::Cursor::Resize_Top_Left_Bottom_Right] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
        cursors[Window::Cursor::Resize_Bottom_Left_Top_Right] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
        cursors[Window::Cursor::Not_Allowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
        cursors[Window::Cursor::Resize_All] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        cursors[Window::Cursor::Resize_Top_Left_Bottom_Right] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        cursors[Window::Cursor::Resize_Bottom_Left_Top_Right] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        cursors[Window::Cursor::Not_Allowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
#endif
        return cursors;
    }();

    return mouse_cursors[cursor_type];
}

void Window::SetMouseCursor(Window::Cursor::Type cursor_type) {
    if (glfwGetInputMode(window_, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        return;

    if (cursor_type == Window::Cursor::None) {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else {
        glfwSetCursor(window_, ToGlfwCursor(cursor_type));
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

}