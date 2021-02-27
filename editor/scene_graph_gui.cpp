#include "scene_graph_gui.h"
#include "engine.h"
#include "editor_gui.h"
#include "gui.h"
#include "meta_runtime.h"

namespace sprint::editor {

static void EntityPopupMenu(ecs::entity_t id, void* ptr_id, std::vector<EditorGuiCommand>& commands) {
    if (ImGui::MenuItem("Add Child")) {
        gui::SetItemOpen(ptr_id, true);
        commands.emplace_back(CreateEntityCommand{id});
    }

    if (ImGui::MenuItem("Destroy")) {
        //TODO
    }

    ImGui::Separator();

    if (ImGui::BeginMenu("Add Component")) {
        ImGui::MenuItem("Camera");
        ImGui::MenuItem("Mesh");
        ImGui::MenuItem("Collider");
        ImGui::EndMenu();
    }
}

static bool BeginEntityTreeNode(const TransformComponent* comp, bool& accept_drop, ecs::entity_t& selected_id, std::vector<EditorGuiCommand>& commands) {
    static const char* DRAG_DROP_TYPE = "DND";
    static const char* label = "ent %d";
    static const float spacing = 4.0f;

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {2,2});

    const auto id = comp->GetID();
    void* ptr_id = (void*)(intptr_t)comp->GetID();
    char str_ptr[16];
    sprintf(str_ptr,"%u", id);

    bool leaf = comp->GetChildren() == nullptr;
    bool selected = selected_id == id;
    bool open;
    bool hovered = false;

    const float cursor_pos_x = ImGui::GetCursorPosX();
    const float width = ImGui::GetContentRegionAvail().x;
    const float height = ImGui::GetFrameHeight();
    const float button_size = ImGui::GetFrameHeight();
    auto column_width = width - button_size - spacing;

    const ImRect clip_rect(
        ImGui::GetCursorScreenPos(),
        { ImGui::GetCursorScreenPos().x + column_width, ImGui::GetCursorScreenPos().y + height});

    ImGui::Dummy({width, height});
    if (!ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
        hovered = ImGui::IsItemHovered(
            ImGuiHoveredFlags_AllowWhenOverlapped | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);
    }

    ImGui::SameLine(); ImGui::SetCursorPosX(cursor_pos_x);

    if (accept_drop) {
        bool small_target_hovered = false;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - spacing);

        if (const ImGuiPayload* payload = gui::DragDropTarget(ptr_id, DRAG_DROP_TYPE, {column_width, spacing}, small_target_hovered)) {
            uint32_t child_id = *(uint32_t *) payload->Data;
            auto *parent = comp->GetParent();
            ecs::entity_t parent_id = parent ? parent->GetID() : ecs::null;
            commands.emplace_back(SetParentCommand{child_id, parent_id, id });
        }
        ImGui::SameLine();
        ImGui::SetCursorPos({cursor_pos_x, ImGui::GetCursorPosY() + spacing});

        if (!small_target_hovered) {
            if (const ImGuiPayload *payload = gui::DragDropTarget(ptr_id, DRAG_DROP_TYPE, {column_width, height})) {
                uint32_t child_id = *(uint32_t *) payload->Data;
                commands.emplace_back(SetParentCommand{child_id, id, ecs::null});
            }
            ImGui::SameLine(); ImGui::SetCursorPosX(cursor_pos_x);
        }
    }

    ImGui::PushClipRect(clip_rect.Min, clip_rect.Max, true);
    ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_SpanAvailWidth |
                            ImGuiTreeNodeFlags_AllowItemOverlap |
                            ImGuiTreeNodeFlags_OpenOnArrow |
                            ImGuiTreeNodeFlags_FramePadding;
    if (leaf) flag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (selected) flag |= ImGuiTreeNodeFlags_Selected;

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0, spacing });
    open = ImGui::TreeNodeEx(ptr_id, flag, label, id);
    ImGui::PopStyleVar();

    // drag and drop source
    if (!ImGui::GetCurrentWindow()->SkipItems) {
        if (ImGui::BeginDragDropSource(
            ImGuiDragDropFlags_AcceptNoPreviewTooltip |
                ImGuiDragDropFlags_SourceNoHoldToOpenOthers |
                ImGuiDragDropFlags_AcceptNoDrawDefaultRect)) {
            accept_drop = false;
            ImGui::SetDragDropPayload(DRAG_DROP_TYPE, &ptr_id, sizeof(uint32_t));
            ImGui::Text(label, id);
            ImGui::EndDragDropSource();
        }
    }

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
        selected_id = id;
    }

    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        ImGui::OpenPopup(str_ptr);
    }

    if (ImGui::BeginPopup(str_ptr)) {
        EntityPopupMenu(id, ptr_id, commands);
        ImGui::EndPopup();
    }

    ImGui::PopClipRect();

    // Add Child button
    if (hovered) {
        ImGui::SameLine(clip_rect.Max.x + spacing);
        auto btn_flags = ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_AlignTextBaseLine |
            ImGuiButtonFlags_NoHoldingActiveId | ImGuiButtonFlags_NoHoveredOnFocus | ImGuiButtonFlags_AllowItemOverlap;

        if (ImGui::ButtonEx("+", ImVec2(button_size, button_size), btn_flags)) {
            gui::SetItemOpen(ptr_id, true);
            commands.emplace_back(CreateEntityCommand{id});
        }
    }

    ImGui::PopStyleVar(1);

    return open && !leaf;
}

static void EndEntityTreeNode() {

    ImGui::TreePop();
}

static void DrawTreeRecursive(TransformComponent::iterator it, bool accept_drop, ecs::entity_t& selected_id, std::vector<EditorGuiCommand>& commands) {
    while (it != nullptr) {
        bool drop = accept_drop;
        if (BeginEntityTreeNode(it.operator->(), drop, selected_id, commands)) {
            DrawTreeRecursive(it->GetChildren(), drop, selected_id, commands);
            EndEntityTreeNode();
        }
        it++;
    }
}

std::unique_ptr<SceneGraphEditorGui> SceneGraphEditorGui::Create(EditorGui& editor, Engine& engine) {
    return std::make_unique<SceneGraphEditorGui>(editor, engine);
}

void SceneGraphEditorGui::OnGui() {
    ecs::entity_t selected = editor_.Selected();
    ImGui::Begin("Scene Graph");
    DrawTreeRecursive(engine_.get_scene()->GetRoots(), true, selected, commands_);
    ImGui::End();

    editor_.Select(selected);

    for (const auto& command : commands_) {
        std::visit([&](const auto& cmd) {
                cmd.Execute(engine_);
            }, command);
    }
    commands_.clear();
}

void SetParentCommand::Execute(Engine &engine) const {
    Scene* scene = engine.get_scene();
    auto& child = scene->get<TransformComponent>(entity);
    auto* parent_transform = parent != ecs::null ? &scene->get<TransformComponent>(parent) : nullptr;
    auto* next_transform = next != ecs::null ? &scene->get<TransformComponent>(next) : nullptr;

    if (parent_transform && parent_transform->IsChildOf(child)) {
        log::core::Error("Invalid operation");
        return;
    }

    if (next_transform && child.GetID() == next_transform->GetID()) {
        log::core::Error("Invalid operation");
        return;
    }

    child.SetParent(parent_transform, next_transform);
}

void CreateEntityCommand::Execute(Engine &engine) const {
    Scene* scene = engine.get_scene();
    auto* parent_transform = parent != ecs::null ? &scene->get<TransformComponent>(parent) : nullptr;
    scene->CreateEntity({}, parent_transform);
}

}