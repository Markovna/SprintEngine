
#include "gui.h"
#include <imgui_internal.h>

namespace sprint::gui {

const ImGuiPayload* DragDropTarget(void *ptr_id, const char *type, ImVec2 size) {
    bool hovered = false;
    return DragDropTarget(ptr_id, type, size, hovered);
}

const ImGuiPayload* DragDropTarget(void *ptr_id, const char *type, ImVec2 size, bool& hovered) {
    hovered = false;

    ImGui::PushID(ptr_id);
    ImGui::Dummy(size);
    ImGui::PopID();
    ImRect rect = {ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};

    const ImGuiPayload* payload_delivery = nullptr;
    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload(type, ImGuiDragDropFlags_AcceptBeforeDelivery)) {
            hovered = true;
            ImGui::RenderFrame(rect.Min, rect.Max, ImGui::GetColorU32(ImGuiCol_ButtonHovered), true);
            if (payload->Delivery) {
                payload_delivery = payload;
            }
        }
        ImGui::EndDragDropTarget();
    }
    return payload_delivery;
}

void SetItemOpen(void *ptr_id, bool open) {
    ImGuiContext &g = *GImGui;
    ImGuiWindow *window = g.CurrentWindow;
    ImGuiStorage *storage = window->DC.StateStorage;
    storage->SetInt(ImGui::GetID(ptr_id), open);
}

bool IntField(const char *label, int *v, int step, int step_fast, ImGuiInputTextFlags flags) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    FieldLabel(label);

    SetNextItemWidth(ImMax(20.0f, GetContentRegionAvail().x));
    bool changed = InputInt("", v, step, step_fast, flags);

    PopID();
    EndGroup();
    return changed;
}

bool FloatField(const char *label,
                float *v,
                float v_speed,
                float v_min,
                float v_max,
                const char *format,
                ImGuiSliderFlags flags) {

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    FieldLabel(label);

    SetNextItemWidth(ImMax(20.0f, GetContentRegionAvail().x));
    bool changed = DragFloat("", v, v_speed, v_min, v_max, format, flags);

    PopID();
    EndGroup();
    return changed;
}

bool DoubleField(const char *label,
                 double *v,
                 double step,
                 double step_fast,
                 const char *format,
                 ImGuiInputTextFlags flags) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    FieldLabel(label);

    SetNextItemWidth(ImMax(20.0f, GetContentRegionAvail().x));
    bool changed = InputDouble("", v, step, step_fast, format, flags);

    PopID();
    EndGroup();
    return changed;
}

bool BoolField(const char *label, bool *v) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    FieldLabel(label);

    SetNextItemWidth(ImMax(20.0f, GetContentRegionAvail().x));
    bool changed = Checkbox("", v);

    PopID();
    EndGroup();
    return changed;
}

void FieldLabel(const char *label) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return;

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;
    const float frame_size = GetFrameHeight();
    float label_width = ImMax(frame_size, GetContentRegionAvail().x / 2.5f);
    SetNextItemWidth(label_width);
    LabelText("", "%s", label);
    SameLine(label_width, style.ItemSpacing.x);
}

bool Vector3Field(const char *label, float* x, float *y, float *z, const char *format, ImGuiSliderFlags flags) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    ImGuiContext& g = *GImGui;
    ImGuiStyle& style = g.Style;

    FieldLabel(label);

    bool changed = false;
    float item_w = (GetContentRegionAvail().x - 2.0f * style.ItemSpacing.y) / 3.0f;
    SetNextItemWidth(item_w);
    PushID("x");
    changed |= DragFloat("", x, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    SameLine(0.0f, style.ItemSpacing.y);
    SetNextItemWidth(item_w);
    PushID("y");
    changed |= DragFloat("", y, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    SameLine(0.0f, style.ItemSpacing.y);
    SetNextItemWidth(GetContentRegionAvail().x);
    PushID("z");
    changed |= DragFloat("", z, 1.0f, 0.0f, 0.0f, format, flags);
    PopID();

    PopID();
    EndGroup();
    return changed;
}

bool TextField(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, ImGuiInputTextCallback callback, void* user_data) {
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
    PushID(label);

    FieldLabel(label);

    SetNextItemWidth(ImMax(20.0f, GetContentRegionAvail().x));
    bool changed = InputText("", buf, buf_size, flags, callback, user_data);

    PopID();
    EndGroup();
    return changed;
}

}