#pragma once

//#include <imgui.h>
//#include <GLFW/glfw3.h>

#include "imgui_impl.h"
#include "KeyCode.h"

namespace sprint {

class ImGuiRenderer {
public:
    ImGuiRenderer();
    ~ImGuiRenderer();

    void BeginFrame(Window* window, float delta_time);
    void EndFrame();

private:
    void Render(ImDrawData* draw_data);
};

}

