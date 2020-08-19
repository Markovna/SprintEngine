#include "Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Window.h"
#include "Shader.h"

void PrepareRenderTriangles();
void RenderTriangle();

Window::Window(int width, int height) {
    int status = glfwInit();
    assert(status); // TODO: asset macro

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwSetErrorCallback([](int error_code, const char* description) {
        Log::CoreError("GLFW error ({1}): {0}", description, error_code);
    });

    m_Window = glfwCreateWindow(width, height, "SprintEngine", NULL, NULL);
    assert(m_Window); // TODO: asset macro

    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, this);

    int loaded = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    assert(loaded); // TODO: asset macro

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
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    RenderTriangle();

    glfwSwapBuffers(m_Window);
    glfwPollEvents();
}

unsigned int vertexArrayObj;
unsigned int elementArrayObj;

std::shared_ptr<Shader> m_Shader;

void PrepareRenderTriangles() {

    m_Shader = Shader::Load("TestShader.shader");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    static float vertices[] = {
            // positions          // colors
             0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 1.0f, 1.0f,
             0.5f, -0.5f,  0.0f,  0.0f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f,  0.0f,  1.0f, 0.0f, 1.0f, 1.0f,
            -0.5f,  0.5f,  0.0f,  1.0f, 1.0f, 0.0f, 1.0f
    };
    unsigned int indices[] = {
            0, 1, 3,
            1, 2, 3
    };

    unsigned int vertexBufferObj;
    glGenVertexArrays(1, &vertexArrayObj);
    glGenBuffers(1, &vertexBufferObj);
    glGenBuffers(1, &elementArrayObj);

    glBindVertexArray(vertexArrayObj);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementArrayObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObj);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(0 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void RenderTriangle() {
    float timeValue = glfwGetTime();
    float greenValue = (sin(timeValue) / 1.0f) + 0.5f;
    float blueValue  = (cos(timeValue) / 1.0f) + 0.5f;
    float redValue   = (sin(timeValue + 3.14f) / 1.0f) + 0.5f;

    m_Shader->Use();
    m_Shader->SetFloat4("mainColor", Vec4(redValue, greenValue, blueValue, 1.0f));

    glBindVertexArray(vertexArrayObj);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}
