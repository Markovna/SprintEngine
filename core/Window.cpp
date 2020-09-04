#include "Log.h"

#include <cmath>

#include "Window.h"
#include "Shader.h"
#include "Vector.h"
#include "Matrix.h"
#include "Texture.h"
#include "Camera.h"
#include "Color.h"


namespace Sprint {

void PrepareRenderTriangles(int, int);
void RenderTriangle();

static void GLFWErrorCallback(int error_code, const char* description) {
    Log::CoreError("GLFW error ({1}): {0}", description, error_code);
}

Window::Window(int width, int height) : m_Width(width), m_Height(height) {
    int status = glfwInit();
    assert(status); // TODO: assert macro
    glfwSetErrorCallback(&GLFWErrorCallback);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(m_Width, m_Height, "SprintEngine", NULL, NULL);
    assert(m_Window); // TODO: assert macro

    glfwMakeContextCurrent(m_Window);

    glfwSetWindowUserPointer(m_Window, this);

    // set input callbacks
    glfwSetKeyCallback(m_Window, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
        Window* window = (Window*) glfwGetWindowUserPointer(w);
        if (action == GLFW_PRESS) {
            window->PushEvent(WindowEvent(KeyEvent(key), true));
        }
        else if (action == GLFW_RELEASE) {
            window->PushEvent(WindowEvent(KeyEvent(key), false));
        }
    });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *w, int button, int action, int mods) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        if (action == GLFW_PRESS) {
            window->PushEvent(WindowEvent(MouseEvent(button), true));
        }
        else if (action == GLFW_RELEASE) {
            window->PushEvent(WindowEvent(MouseEvent(button), false));
        }
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow *w, double xpos, double ypos) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        window->PushEvent(WindowEvent(MouseMoveEvent(Vec2(xpos, ypos))));
    });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *w) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        window->PushEvent(WindowEvent(CloseEvent()));
    });

    GL::Init();

    PrepareRenderTriangles(m_Width, m_Height);
}

Window::~Window() {
    glfwDestroyWindow(m_Window);
    glfwTerminate();

    GL::Shutdown();
}

void Window::OnUpdate() {
    glfwMakeContextCurrent(m_Window);

    GL::Clear(Color(0.2f, 0.2f, 0.2f, 1.0f), GL::CLEAR_COLOR | GL::CLEAR_DEPTH);

    RenderTriangle();

    GL::SwapBuffers();

    glfwPollEvents();
}

bool Window::PollEvent(Event& event) {
    if (m_Events.empty())
        return false;

    event = m_Events.front();
    m_Events.pop();
    return true;
}

void Window::PushEvent(Event event) {
    m_Events.push(std::move(event));
}

GL::VertexBufferHandle m_VBHandle;
std::shared_ptr<Shader> m_Shader;
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

    m_Camera   = std::make_shared<Camera>(Matrix::Perspective(60.0f * M_PI / 180.0f, (float)width, (float)height, 0.1f, 100.0f));
    m_Texture  = Texture::Load("assets/textures/container.jpg");
    m_Texture2 = Texture::Load("assets/textures/seal.png");
    m_Shader   = Shader::Load(
            "assets/shaders/TestShader.shader",
            {
                GL::AttributeType::POSITION, GL::AttributeType::TEXCOORD0
            });

    if (m_Texture) m_Texture->Bind(0);
    if (m_Texture2) m_Texture2->Bind(1);

    m_Shader->SetInt("Texture1", 0);
    m_Shader->SetInt("Texture2", 1);

    size_t count;
    float* vertices = GetVertices();
    uint32_t* indices = GetIndices(count);

    m_VBHandle = GL::CreateVertexBuffer(
            vertices, 5 * count,
            {
                {GL::AttributeType::POSITION, GL::AttributeFormat::Vec3 },
                {GL::AttributeType::TEXCOORD0, GL::AttributeFormat::Vec2 }
            }
        );
}

void RenderTriangle() {
    float timeValue =  0.5f * glfwGetTime();

    Color color(sin(timeValue) / 2.0f + 0.5f,0.3f,cos(timeValue) / 2.0f + 0.5f,1.0f);

    Matrix model = Matrix::Identity;
    model *= Matrix::Rotation(Quat(Vec3::Forward, 1.0f * timeValue));
    model *= Matrix::Rotation(Quat(Vec3::Left, 1.25f * timeValue));
    model *= Matrix::Translation(Vec3::Zero);

    m_Camera->SetPosition(Vec3(0.0f, 0.0f, 3.0f));
    m_Camera->SetRotation(Quat(Vec3::Right, 0.0f * M_PI / 180.0f));

    m_Shader->SetFloat4("mainColor", color);
    m_Shader->SetMat("model", model);
    m_Shader->SetMat("view", m_Camera->GetViewMatrix());
    m_Shader->SetMat("projection", m_Camera->GetProjectionMatrix());

    GL::Bind(m_VBHandle);
    m_Shader->Render();
}

}