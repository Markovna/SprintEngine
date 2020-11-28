#pragma once

#include <imgui.h>
#include <KeyCode.h>

#include <unordered_map>
#include <Window.h>

namespace sprint {

namespace gui {
using namespace ImGui;
}

namespace imgui_impl {

void Init();
void Shutdown();

void NewFrame(Window* const window, float delta_time);

}

}


