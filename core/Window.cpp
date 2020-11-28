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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
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

void Window::OnUpdate() {

    PrepareRenderTriangles(width_, height_);

    gl::Clear(Color(0.2f, 0.2f, 0.2f, 1.0f), gl::kClearColor | gl::kClearDepth);

    RenderTriangle();

    gl::SwapBuffers();

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

gl::VertexBufferHandle m_VBHandle;
std::unique_ptr<Shader> m_Shader;
std::shared_ptr<Texture> m_Texture;
std::shared_ptr<Texture> m_Texture2;
std::shared_ptr<Camera> m_Camera;

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
    if (m_Camera)
        return;

    m_Camera   = std::make_shared<Camera>(Matrix::Perspective(60.0f * M_PI / 180.0f, (float)width, (float)height, 0.1f, 100.0f));
    m_Texture  = Texture::Load("assets/textures/container.jpg");
    m_Texture2 = Texture::Load("assets/textures/seal.png");
    m_Shader   = std::make_unique<Shader>(Shader::Load(
                    "assets/shaders/TestShader.shader",
                    {
                            gl::AttributeType::POSITION,
                            gl::AttributeType::TEXCOORD0
                    })
                );

    if (m_Texture) {
        gl::Bind(m_Texture->get_handle(), 0);
    }

    if (m_Texture2) {
        gl::Bind(m_Texture2->get_handle(), 1);
    }

    gl::SetInt(m_Shader->get_handle(), "Texture1", 0);
    gl::SetInt(m_Shader->get_handle(), "Texture2", 1);

    size_t count;
    float* vertices = GetVertices();
    uint32_t* indices = GetIndices(count);

    m_VBHandle = gl::CreateVertexBuffer(
        (void*) vertices,
        5 * count,
        {
            {gl::AttributeType::POSITION,  gl::AttributeFormat::Vec3 },
            {gl::AttributeType::TEXCOORD0, gl::AttributeFormat::Vec2 }
        }
    );
}

void RenderTriangle() {
    float timeValue =  0.5 * glfwGetTime();

    Color color(sin(timeValue) / 2.0f + 0.5f,0.3f,cos(timeValue) / 2.0f + 0.5f,1.0f);

    Matrix model = Matrix::Identity;
    model *= Matrix::Rotation(Quat(Vec3::Forward, 1.0f * timeValue));
    model *= Matrix::Rotation(Quat(Vec3::Left, 1.25f * timeValue));
    model *= Matrix::Translation(Vec3::Zero);

    m_Camera->set_position(Vec3(0.0f, 0.0f, 3.0f));
    m_Camera->set_rotation(Quat(Vec3::Right, 0.0f * M_PI / 180.0f));

    gl::SetFloat4(m_Shader->get_handle(), "mainColor", color);
    gl::SetMat(m_Shader->get_handle(), "model", model);
    gl::SetMat(m_Shader->get_handle(), "view", m_Camera->GetViewMatrix());
    gl::SetMat(m_Shader->get_handle(), "projection", m_Camera->get_projection_matrix());

    gl::Bind(m_VBHandle);
    gl::Render(m_Shader->get_handle());
}

}