#include <components/camera.h>
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

static void BeginEntityTreeNode(
    const TransformComponent* comp,
    float spacing,
    ecs::entity_t& selected_id,
    std::vector<EditorGuiCommand>& commands,
    bool& opened,
    bool& hovered) {

    static const char* DRAG_DROP_TYPE = "DND";
    static const char* label = "ent %d";

    const auto id = comp->GetID();
    void* ptr_id = (void*)(intptr_t)comp->GetID();
    char str_ptr[16];
    sprintf(str_ptr,"%u", id);

    bool leaf = comp->GetChildren() == nullptr;
    bool selected = selected_id == id;
    bool open;
    hovered = false;

    const ImVec2 cursor_pos = ImGui::GetCursorPos();
    const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    const float min_x = ImGui::GetWindowContentRegionMin().x;
    const ImVec2 size = {ImGui::GetWindowContentRegionWidth(), ImGui::GetFrameHeight()};

    hovered =
        !ImGui::IsMouseDragging(ImGuiMouseButton_Left) &&
        ImGui::IsMouseHoveringRect(screen_pos, {screen_pos.x + size.x, screen_pos.y + size.y}, false);

    bool small_target_hovered = false;
    ImGui::SetCursorPos({min_x, cursor_pos.y - spacing});
    if (const ImGuiPayload* payload = gui::DragDropTarget(ptr_id, DRAG_DROP_TYPE, {size.x, spacing}, small_target_hovered)) {
        uint32_t child_id = *(uint32_t *) payload->Data;
        auto *parent = comp->GetParent();
        ecs::entity_t parent_id = parent ? parent->GetID() : ecs::null;
        commands.emplace_back(SetParentCommand{child_id, parent_id, id });
    }

    ImGui::SetCursorPos({min_x, cursor_pos.y});

    if (!small_target_hovered) {
        if (const ImGuiPayload *payload = gui::DragDropTarget(ptr_id, DRAG_DROP_TYPE, size)) {
            uint32_t child_id = *(uint32_t *) payload->Data;
            commands.emplace_back(SetParentCommand{child_id, id, ecs::null});
        }
    }

    ImGui::SetCursorPos(cursor_pos);
    ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_SpanAvailWidth |
                            ImGuiTreeNodeFlags_SpanFullWidth |
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

    opened = open && !leaf;
}

static void EndEntityTreeNode() {

    ImGui::TreePop();
}

enum class VisitTreeNodesResult {
    Recursive,
    Continue,
    Break
};

template <class F>
static void VisitTreeNodes(TransformComponent::iterator it, F function) {
    if (it == nullptr)
        return;

    VisitTreeNodesResult result = function(it);
    if (result == VisitTreeNodesResult::Break)
        return;

    if (result == VisitTreeNodesResult::Recursive) {
        VisitTreeNodes(it->GetChildren(), function);
    }

    VisitTreeNodes(++it, function);
}

void SceneGraphEditorGui::DrawSceneGraph() {

    ecs::entity_t selected = editor_.Selected();
    ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 14.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {2,2});

    const float spacing = 4.0f;
    const float button_size = ImGui::GetFrameHeight();
    const ImVec2 screen_pos = ImGui::GetCursorScreenPos();
    const ImRect clip_rect = {
        screen_pos,
        {
            screen_pos.x + ImGui::GetContentRegionAvail().x - button_size - spacing,
            screen_pos.y + ImGui::GetWindowHeight()
        }
    };

    ImGui::PushClipRect(clip_rect.Min, clip_rect.Max, true);

    ecs::entity_t hovered_id;
    ImVec2 button_pos;
    VisitTreeNodes(engine_.get_scene()->GetRoots(), [&] (TransformComponent::iterator it) {
        bool opened, hovered;

        BeginEntityTreeNode((TransformComponent*) it, spacing, selected, commands_, opened, hovered);
        if (hovered) {
            hovered_id = it->GetID();
            button_pos = {
                ImGui::GetItemRectMax().x - button_size,
                ImGui::GetItemRectMin().y
            };
        }

        if (opened)
            return VisitTreeNodesResult::Recursive;

        while (it && !it->GetNext()) {
            EndEntityTreeNode();
            it = TransformComponent::iterator(it->GetParent());
        }

        return VisitTreeNodesResult::Continue;
    });

    ImGui::PopClipRect();

    if (hovered_id != ecs::null) {
        ImGui::SetCursorScreenPos(button_pos);
        auto btn_flags = ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_AlignTextBaseLine |
            ImGuiButtonFlags_NoHoldingActiveId | ImGuiButtonFlags_NoHoveredOnFocus | ImGuiButtonFlags_AllowItemOverlap;
        ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetColorU32(ImGuiCol_ButtonHovered));
        if (ImGui::ButtonEx("+", ImVec2(button_size, button_size), btn_flags)) {
            gui::SetItemOpen((void*)(intptr_t)hovered_id, true);
            commands_.emplace_back(CreateEntityCommand{hovered_id});
        }
        ImGui::PopStyleColor();
    }

    gui::PopStyleVar(2);
    
    if (ImGui::IsWindowFocused() && ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowHovered()) {
        selected = ecs::null;
    }

    editor_.Select(selected);
}

std::unique_ptr<SceneGraphEditorGui> SceneGraphEditorGui::Create(EditorGui& editor, Engine& engine) {
    return std::make_unique<SceneGraphEditorGui>(editor, engine);
}

void SceneGraphEditorGui::OnGui() {

    gui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 4));
    ImGui::Begin("Scene Graph");
    gui::PopStyleVar();

    DrawSceneGraph();

    ImGui::End();

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