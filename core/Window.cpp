#include "Log.h"

#include <cmath>

#include "Shader.h"
#include "Window.h"
#include "Vector.h"
#include "Matrix.h"
#include "Texture.h"
#include "Camera.h"
#include "color.h"
#include "gfx.h"
#include "Application.h"

namespace sprint {

void PrepareRenderTriangles(int, int);
void RenderTriangle();

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

    glfwSetWindowUserPointer(window_, this);

    // set input callbacks
    glfwSetKeyCallback(window_, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
        Window* window = (Window*) glfwGetWindowUserPointer(w);
        if (action == GLFW_PRESS) {
            window->PushEvent(WindowEvent(KeyEvent(key), true));
        }
        else if (action == GLFW_RELEASE) {
            window->PushEvent(WindowEvent(KeyEvent(key), false));
        }
    });

    glfwSetMouseButtonCallback(window_, [](GLFWwindow *w, int button, int action, int mods) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        if (action == GLFW_PRESS) {
            window->PushEvent(WindowEvent(MouseEvent(button), true));
        }
        else if (action == GLFW_RELEASE) {
            window->PushEvent(WindowEvent(MouseEvent(button), false));
        }
    });

    glfwSetCursorPosCallback(window_, [](GLFWwindow *w, double xpos, double ypos) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        window->PushEvent(WindowEvent(MouseMoveEvent(Vec2(xpos, ypos))));
    });

    glfwSetWindowCloseCallback(window_, [](GLFWwindow *w) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        window->PushEvent(WindowEvent(CloseEvent()));
    });
}

Window::~Window() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void RenderTrianglezzz();

void Window::OnUpdate() {

    PrepareRenderTriangles(width_, height_);

    RenderTrianglezzz();

    gfx::Frame();

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
    events_.push(event);
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
//    if (glfwGetInputMode(window_, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
//        return;
    if (cursor_type == Window::Cursor::None) {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }
    else {
        glfwSetCursor(window_, ToGlfwCursor(cursor_type));
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

std::unique_ptr<Shader> m_Shader;
std::shared_ptr<Texture> m_Texture1;
std::shared_ptr<Texture> m_Texture2;

gfx::VertexBufferHandle vb_handle;
gfx::IndexBufferHandle ib_handle;


float* GetVertices() {
    static float vertices[] = {
            // pos                // tex coords
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
        };

    return vertices;
}

uint32_t* GetIndices(size_t& count) {
    static uint32_t indices[] = {
            0,  1,  2,
            3,  4,  5,
            6,  7,  8,
            9,  10, 11,
            12, 13, 14,
            15, 16, 17,
            18, 19, 20,
            21, 22, 23,
            24, 25, 26,
            27, 28, 29,
            30, 31, 32,
            33, 34, 35
        };
    count = 36;
    return indices;
}

void PrepareRenderTriangles(int width, int height) {
    static bool inited = false;
    if (inited) return;

    inited = true;

    m_Texture1  = Texture::Load("assets/textures/container.jpg");
    m_Texture2 = Texture::Load("assets/textures/seal.png");
    m_Shader   = std::make_unique<Shader>(Shader::Load(
                    "assets/shaders/TestShader.shader",
                    {
                            gfx::Attribute::Binding::POSITION,
                            gfx::Attribute::Binding::TEXCOORD0
                    })
                );


    Matrix view_mat = Matrix::Translation(Vec3(0.0f, 0.0f, 3.0f)) * Matrix::Rotation(Quat(Vec3::Right, 0.0f * M_PI / 180.0f));
    gfx::SetView(0, view_mat);
    gfx::SetProjection(0, Matrix::Perspective(60.0f * M_PI / 180.0f, (float)width, (float)height, 0.1f, 100.0f));
    gfx::SetClear(0, gfx::ClearFlag::Color | gfx::ClearFlag::Depth);
    gfx::SetClearColor(0, Color(0.1, 0.1, 0.1, 1.0));

    gfx::SetUniform(m_Shader->get_handle(), "Texture1", m_Texture1->get_handle(), 0);
    gfx::SetUniform(m_Shader->get_handle(), "Texture2", m_Texture2->get_handle(), 1);

    size_t count;
    float* vertices = GetVertices();
    uint32_t* indices = GetIndices(count);

    vb_handle = gfx::CreateVertexBuffer(
        (void*) vertices,
        count,
        {
            {gfx::Attribute::Binding::POSITION,  gfx::Attribute::Format::Vec3 },
            {gfx::Attribute::Binding::TEXCOORD0, gfx::Attribute::Format::Vec2 }
        }
    );

    ib_handle = gfx::CreateIndexBuffer(indices, count);
}

void RenderTriangle(float timeValue, Vec3 position) {

    Color color(sin(timeValue) / 2.0f + 0.5f,0.3f,cos(timeValue) / 2.0f + 0.5f,1.0f);

    Matrix model = Matrix::Identity;
    model *= Matrix::Rotation(Quat(Vec3::Forward, 1.0f * timeValue));
    model *= Matrix::Rotation(Quat(Vec3::Left, 1.25f * timeValue));
    model *= Matrix::Translation(position);

    gfx::SetUniform(m_Shader->get_handle(), "Texture1", m_Texture1->get_handle(), 0);
    gfx::SetUniform(m_Shader->get_handle(), "Texture2", m_Texture2->get_handle(), 1);

    gfx::SetUniform(m_Shader->get_handle(), "mainColor", color);
    gfx::SetTransform(model);
    gfx::SetBuffer(vb_handle);
    gfx::SetBuffer(ib_handle);

    gfx::Render(0, m_Shader->get_handle());
}

void RenderTrianglezzz() {
    float timeValue =  0.5f * glfwGetTime();
    RenderTriangle(timeValue, Vec3::Up);

    for (int i = 1; i < 25; i++) {
        RenderTriangle(timeValue + i * 0.1, (i * 1.5 + 1) * Vec3::Up + (i * 0.6)*Vec3::Forward);
        RenderTriangle(timeValue - i * 0.1, (i * 1.5 + 1) * Vec3::Up + (i * 0.6)*Vec3::Backward);
        RenderTriangle(timeValue + i * 0.1, (i * 1.5 + 1) * Vec3::Up + (i * 0.6)*Vec3::Right);
        RenderTriangle(timeValue - i * 0.1, (i * 1.5 + 1) * Vec3::Up + (i * 0.6)*Vec3::Left);
    }
}

}