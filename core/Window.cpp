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
            window->PushEvent(WindowEvent(
                KeyEvent{
                    (KeyCode)key,
                    bool(mods & GLFW_MOD_CONTROL),
                    bool(mods & GLFW_MOD_SHIFT),
                    bool(mods & GLFW_MOD_ALT),
                    bool(mods & GLFW_MOD_SUPER)
                    },
                    true));
        }
        else if (action == GLFW_RELEASE) {
            window->PushEvent(WindowEvent(
                KeyEvent{
                    KeyCode(key),
                    bool(mods & GLFW_MOD_CONTROL),
                    bool(mods & GLFW_MOD_SHIFT),
                    bool(mods & GLFW_MOD_ALT),
                    bool(mods & GLFW_MOD_SUPER)
                    },
                   false));
        }
    });

    glfwSetMouseButtonCallback(window_, [](GLFWwindow *w, int button, int action, int mods) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        if (action == GLFW_PRESS) {
            window->PushEvent(WindowEvent(MouseEvent{(MouseCode)button}, true));
        }
        else if (action == GLFW_RELEASE) {
            window->PushEvent(WindowEvent(MouseEvent{(MouseCode)button}, false));
        }
    });

    glfwSetScrollCallback(window_, [](GLFWwindow* w, double xoffset, double yoffset){
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        window->PushEvent(WindowEvent(ScrollEvent { Vec2(float(xoffset), float(yoffset)) }));
    });

    glfwSetCursorPosCallback(window_, [](GLFWwindow *w, double xpos, double ypos) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        window->PushEvent(WindowEvent(MouseMoveEvent { Vec2(float(xpos), float(ypos)) }));
    });

    glfwSetWindowCloseCallback(window_, [](GLFWwindow *w) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        window->PushEvent(WindowEvent(CloseEvent()));
    });

    glfwSetCharCallback(window_, [](GLFWwindow* w, unsigned int codepoint) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        window->PushEvent(WindowEvent(TextEvent { codepoint } ));
    });
}

void ClearResources();

Window::~Window() {
    glfwDestroyWindow(window_);
    glfwTerminate();

    ClearResources();
}

void RenderTrianglezzz();

void Window::OnUpdate() {

    PrepareRenderTriangles(width_, height_);

    RenderTrianglezzz();

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

static std::unique_ptr<Shader> m_Shader;
static std::shared_ptr<Texture> m_Texture1;
static std::shared_ptr<Texture> m_Texture2;
static gfx::VertexBufferHandle vb_handle;
static gfx::IndexBufferHandle ib_handle;
static Matrix view_mat;
static Matrix proj_mat;

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


    view_mat = Matrix::Translation(Vec3(0.0f, 0.0f, -3.0f));
    proj_mat = Matrix::Perspective(60.0f * M_PI / 180.0f, (float)width, (float)height, 0.01f, 100.0f);
//    proj_mat = Matrix::Ortho((float)width*0.05f, (float)height*0.05f, 0.1f, 100.0f);

    gfx::SetView(0, Matrix::GetInverse(view_mat));
    gfx::SetProjection(0, proj_mat);
    gfx::SetClear(0, gfx::ClearFlag::Color | gfx::ClearFlag::Depth);
    gfx::SetClearColor(0, Color(0.1, 0.1, 0.1, 1.0));

    Application::OnKeyPress.connect([](KeyEvent& key){
        if (key.key_code == key::Left || key.key_code == key::Right) {
            float a = key.key_code == key::Left ? -1 : 1;
            view_mat = Matrix::Rotation(Quat(Vec3::Right, a * 0.01f * M_PI)) * view_mat;
            gfx::SetView(0, Matrix::GetInverse(view_mat));
        }

        else if (key.key_code == key::Up || key.key_code == key::Down || key.key_code == key::W || key.key_code == key::S) {
            view_mat = Matrix::Translation((key.key_code == key::Up || key.key_code == key::W ? 0.1f : -0.1f) * Vec3::Up) * view_mat;
            gfx::SetView(0, Matrix::GetInverse(view_mat));
        }

        else if (key.key_code == key::A || key.key_code == key::D) {
            view_mat = Matrix::Translation((key.key_code == key::A ? -0.1f : 0.1f) * Vec3::Forward) * view_mat;
            gfx::SetView(0, Matrix::GetInverse(view_mat));
        }
    });

    gfx::SetUniform(m_Shader->get_handle(), "Texture1", m_Texture1->get_handle(), 0);
    gfx::SetUniform(m_Shader->get_handle(), "Texture2", m_Texture2->get_handle(), 1);

    size_t count;
    float* vertices = GetVertices();
    uint32_t* indices = GetIndices(count);

    vb_handle = gfx::CreateVertexBuffer(
        gfx::MakeRef(vertices, sizeof(float) * 5 * count),
        count,
        {
            {gfx::Attribute::Binding::POSITION,  gfx::Attribute::Format::Vec3 },
            {gfx::Attribute::Binding::TEXCOORD0, gfx::Attribute::Format::Vec2 }
        }
    );

    ib_handle = gfx::CreateIndexBuffer(gfx::MakeRef(indices, sizeof(uint32_t) * count ), count);
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
//    gfx::SetScissor({ 0, 0, 300, 300 });

    gfx::Render(0, m_Shader->get_handle());
}

void RenderTrianglezzz() {

    float timeValue =  0.5f * (float) glfwGetTime();

    const float radius = 10.0f;
    float camX = sin(timeValue) * radius;
    float camZ = cos(timeValue) * radius;
//    view_mat = Matrix::LookAt(Vec3(camX, 0.0, camZ), Vec3(0.0, 0.0, 0.0),Vec3(0.0, 1.0, 0.0));
//    gfx::SetView(0, Matrix::GetInverse(view_mat));

    RenderTriangle(timeValue, Vec3::Zero);
    RenderTriangle(timeValue, 2 * Vec3::Right);
//    RenderTriangle(timeValue, 2 * Vec3::Backward);
//    RenderTriangle(timeValue, 2 * Vec3::Up);

    for (int i = 1; i < 25; i++) {
        RenderTriangle(timeValue + i * 0.1f, (i * 2.0f + 1) * Vec3::Up + (i * 0.7f) * Vec3::Forward);
        RenderTriangle(timeValue - i * 0.1f, (i * 2.0f + 1) * Vec3::Up + (i * 0.7f) * Vec3::Backward);
    }
}

void ClearResources() {
    m_Texture1.reset();
    m_Texture2.reset();
    m_Shader.reset();

    gfx::Destroy(vb_handle);
    gfx::Destroy(ib_handle);
}

}