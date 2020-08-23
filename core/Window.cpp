#include "Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>

#include "Window.h"
#include "Shader.h"
#include "Vector.h"
#include "Matrix.h"
#include "Texture.h"
#include "stb_image.h"

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

unsigned int vertexArrayObj;
unsigned int elementArrayObj;

std::shared_ptr<Shader> m_Shader;
std::shared_ptr<Texture> texture;
std::shared_ptr<Texture> texture2;

void PrepareRenderTriangles() {

    glEnable(GL_DEPTH_TEST);

    texture = Texture::Load("assets/textures/container.jpg");
    texture2 = Texture::Load("assets/textures/seal.jpg");

    m_Shader = Shader::Load("assets/shaders/TestShader.shader");
    m_Shader->Use();
    m_Shader->SetInt("Texture1", 0);
    m_Shader->SetInt("Texture2", 1);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    static float vertices[] = {
            // positions          // colors                // tex coords
//             0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f, 1.0f,  1.0f, 1.0f,
//             0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 1.0f,  1.0f, 0.0f,
//            -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f, 1.0f,  0.0f, 0.0f,
//            -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f, 1.0f,  0.0f, 1.0f,

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
//    unsigned int indices[] = {
//            0, 1, 3,
//            1, 2, 3,
//        };

    unsigned int vertexBufferObj;
    glGenVertexArrays(1, &vertexArrayObj);
    glGenBuffers(1, &vertexBufferObj);
    glGenBuffers(1, &elementArrayObj);

    glBindVertexArray(vertexArrayObj);

//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayObj);
//    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void RenderTriangle(int width, int height) {
    float timeValue = glfwGetTime() * 0.5f;

    Vec4 color;
    color.X = sin(timeValue) / 2.0f + 0.5f;
    color.Y = cos(timeValue) / 2.0f + 0.5f;
    color.Z = sin(timeValue + M_PI * 0.5f) / 2.0f + 0.5f;
    color.W = 1.0f;

    Matrix model;
    model *= Matrix::Rotation(Quat(Vec3::Forward, 1.5f * timeValue));
    model *= Matrix::Rotation(Quat(Vec3::Left, 1.0f * timeValue));
    model *= Matrix::Translation(Vec3(0.0f, 0.0f, 1.0f));

    Matrix view = Matrix::Translation(Vec3(0.0f, 0.0f, 3.0f));
    Matrix projection = Matrix::Perspective(45.0f * M_PI / 180.0f, (float)width, (float)height, 0.1f, 100.0f);
//    Matrix projection = Matrix::Ortho(1.0f, width / height, 0.1f, 100.0f);

    m_Shader->Use();
    m_Shader->SetFloat4("mainColor", color);
    m_Shader->SetMat("model", model);
    m_Shader->SetMat("view", view);
    m_Shader->SetMat("projection", projection);

    if (texture)
        texture->Bind(0);

    if (texture2)
        texture2->Bind(1);

    glBindVertexArray(vertexArrayObj);
//    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glUseProgram(0);
}
