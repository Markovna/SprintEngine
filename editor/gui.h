#pragma once

#include <imgui.h>
#include <imgui_internal.h>

namespace sprint::gui {

using namespace ImGui;

const ImGuiPayload* DragDropTarget(void *ptr_id, const char *type, ImVec2 size, bool& hovered);
const ImGuiPayload* DragDropTarget(void *ptr_id, const char *type, ImVec2 size);

void FieldLabel(const char* label);

bool IntField(const char* label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
bool FloatField(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
bool DoubleField(const char* label, double* v, double step = 0.0, double step_fast = 0.0, const char* format = "%.6f", ImGuiInputTextFlags flags = 0);
bool BoolField(const char* label, bool* v);
bool Vector3Field(const char* label, float* x, float *y, float *z, const char* format = "%.3f", ImGuiSliderFlags flags = 0);
bool TextField(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);

void SetItemOpen(void *ptr_id, bool open);

}