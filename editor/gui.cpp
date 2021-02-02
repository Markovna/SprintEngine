
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

}