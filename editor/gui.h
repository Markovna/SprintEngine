#pragma once

#include <imgui.h>

namespace sprint::gui {

using namespace ImGui;


const ImGuiPayload* DragDropTarget(void *ptr_id, const char *type, ImVec2 size, bool& hovered);
const ImGuiPayload* DragDropTarget(void *ptr_id, const char *type, ImVec2 size);

void SetItemOpen(void *ptr_id, bool open);

}