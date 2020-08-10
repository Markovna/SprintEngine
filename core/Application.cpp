#include "Application.h"
#include "Log.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

static void GLFWErrorCallback(int error_code, const char* description) {
    Log::CoreError("GLFW error ({1}): {0}", description, error_code);
}

int Application::Run() {
    Log::Init();

    if (!glfwInit()) {
        Log::CoreError("GLFW could not initialize!");
    }
    else {

        glfwSetErrorCallback(GLFWErrorCallback);

        GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "SprintEngine", NULL, NULL);
        if (!window) {
            Log::CoreError("Window could not be created!");
        }
        else {

            glfwMakeContextCurrent(window);

            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            {
                Log::CoreError("Failed to initialize GLAD");
                return -1;
            }

            // set input callbacks
            glfwSetKeyCallback(window, [](GLFWwindow *w, int key, int scancode, int action, int mods) {
                if (action == GLFW_PRESS)
                    Log::CoreInfo("Key pressed: {0}", key);
                else if (action == GLFW_RELEASE)
                    Log::CoreInfo("Key released: {0}", key);
            });

            glfwSetMouseButtonCallback(window, [](GLFWwindow *w, int button, int action, int mods) {
                if (action == GLFW_PRESS)
                    Log::CoreInfo("Mouse pressed: {0}", button);
                else if (action == GLFW_RELEASE)
                    Log::CoreInfo("Mouse released: {0}", button);
            });

            glfwSetCursorPosCallback(window, [](GLFWwindow * w, double xpos, double ypos) {
                //Log::CoreInfo("Mouse moved: [{0}, {1}]", xpos, ypos);
            });

            glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

            while (!glfwWindowShouldClose(window)) {

                glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT);

                glfwSwapBuffers(window);
                glfwPollEvents();
            }

            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }
    return 0;
}