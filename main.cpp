#include <GLFW/glfw3.h>
#include "Log.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main() {

    Log::Init();

    if (!glfwInit()) {
        Log::CoreError("GLFW could not initialize!");
    }
    else {
        GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "SprintEngine", NULL, NULL);
        if (!window) {
            Log::CoreError("Window could not be created!");
        } else {

            while (!glfwWindowShouldClose(window))
            {
                // do something

                glfwSwapBuffers(window);
                glfwPollEvents();
            }

            glfwDestroyWindow(window);
        }
        glfwTerminate();
    }

    return 0;
}
