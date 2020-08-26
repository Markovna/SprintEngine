#include "Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

#include "Window.h"
#include "Shader.h"
#include "Vector.h"
#include "Matrix.h"
#include "Texture.h"
#include "VertexBuffer.h"

void PrepareRenderTriangles();
void RenderTriangle(int, int);

Window::Window(int width, int height) {
    int status = glfwInit();
    assert(status); // TODO: assert macro

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback([](int error_code, const char* description) {
        Log::CoreError("GLFW error ({1}): {0}", description, error_code);
    });

    m_Window = glfwCreateWindow(width, height, "SprintEngine", NULL, NULL);
    assert(m_Window); // TODO: assert macro

    m_Width = width;
    m_Height = height;

    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, this);

    int loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    assert(loaded); // TODO: assert macro

    // set input callbacks
    glfwSetKeyCallback(m_Window, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        if (!window->m_EventCallback)
            return;

        if (action == GLFW_PRESS) {
            KeyPressEvent event(KeyCode::A); // TODO:
            window->m_EventCallback(event);
        }
        else if (action == GLFW_RELEASE) {
            KeyReleaseEvent event(KeyCode::A); // TODO
            window->m_EventCallback(event);
        }
    });

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow *w, int button, int action, int mods) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        if (!window->m_EventCallback)
            return;

        if (action == GLFW_PRESS) {
            MouseDownEvent event;
            window->m_EventCallback(event);
        }
        else if (action == GLFW_RELEASE) {
            MouseUpEvent event;
            window->m_EventCallback(event);
        }
    });

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow *w, double xpos, double ypos) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        if (window->m_EventCallback) {
            MouseMoveEvent event(xpos, ypos);
            window->m_EventCallback(event);
        }
    });

    glfwSetWindowCloseCallback(m_Window, [](GLFWwindow *w) {
        Window* window = (Window*)glfwGetWindowUserPointer(w);
        if (window->m_CloseCallback)
            window->m_CloseCallback();
    });

    PrepareRenderTriangles();
}

Window::~Window() {
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}


void Window::OnUpdate() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    RenderTriangle(m_Width, m_Height);

    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

std::shared_ptr<VertexArray> m_VertexArray;
std::shared_ptr<VertexBuffer> m_VertexBuffer;
std::shared_ptr<Shader> m_Shader;
std::shared_ptr<Texture> m_Texture;
std::shared_ptr<Texture> m_Texture2;

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

void PrepareRenderTriangles() {

    glEnable(GL_DEPTH_TEST);

    m_Texture  = Texture::Load("assets/textures/container.jpg");
    m_Texture2 = Texture::Load("assets/textures/seal.png");
    m_Shader   = Shader::Load("assets/shaders/TestShader.shader");

    if (m_Texture) m_Texture->Bind(0);
    if (m_Texture2) m_Texture2->Bind(1);

    m_Shader->Bind();
    m_Shader->SetInt("Texture1", 0);
    m_Shader->SetInt("Texture2", 1);

    size_t count;
    float* vertices = GetVertices();
    uint32_t* indices = GetIndices(count);

    m_VertexArray = std::make_shared<VertexArray>();
    m_VertexArray->SetIndexBuffer(indices, count);

    m_VertexBuffer = std::make_shared<VertexBuffer>(
            VertexBuffer::Create(vertices, 5 * count * sizeof(*vertices), { VertexDataType::Vec3, VertexDataType::Vec2 }));

    unsigned int slot;
    if (m_Shader->TryGetAttributeID("aPos", slot)) {
        m_VertexBuffer->SetAttribute(*m_VertexArray, slot, 0);
    }
    if (m_Shader->TryGetAttributeID("aTexCoord", slot)) {
        m_VertexBuffer->SetAttribute(*m_VertexArray, slot, 1);
    }
}

void RenderTriangle(int width, int height) {
    float timeValue =  0.5 * glfwGetTime();

    Vec4 color(sin(timeValue) / 2.0f + 0.5f,0.3f,cos(timeValue) / 2.0f + 0.5f,1.0f);

    Matrix model;
    model *= Matrix::Rotation(Quat(Vec3::Forward, 1.0f * timeValue));
    model *= Matrix::Rotation(Quat(Vec3::Left, 1.25f * timeValue));
    model *= Matrix::Translation(Vec3(0.0f, 0.0f, 1.0f));

    Matrix view = Matrix::Translation(Vec3(0.0f, 0.0f, 2.0f));
    Matrix projection = Matrix::Perspective(60.0f * M_PI / 180.0f, (float)width, (float)height, 0.1f, 100.0f);
//    Matrix projection = Matrix::Ortho(1.0f, width / height, 0.1f, 100.0f);

    m_Shader->Bind();
    m_Shader->SetFloat4("mainColor", color);
    m_Shader->SetMat("model", model);
    m_Shader->SetMat("view", view);
    m_Shader->SetMat("projection", projection);

    m_VertexArray->Bind();
    glDrawElements(GL_TRIANGLES, m_VertexArray->IndexCount(), GL_UNSIGNED_INT, nullptr);
    glUseProgram(0);
}
