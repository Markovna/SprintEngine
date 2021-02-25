#include <imgui.h>
#include <imgui_internal.h>

#include "scene_graph_gui.h"
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

    auto id = comp->GetID();
    void* ptr_id = (void*)(intptr_t)comp->GetID();
    char str_ptr[16];
    sprintf(str_ptr,"%u", id);

    bool leaf = comp->GetChildren() == nullptr;
    bool selected = selected_id == id;
    bool open;
    bool hovered;

    float cursor_pos_x = ImGui::GetCursorPosX();
    float width = ImGui::GetContentRegionAvailWidth();
    float height = ImGui::GetTextLineHeight();
    auto column_width = ImGui::GetContentRegionAvailWidth() - 25;

    ImGui::Dummy({width, height});
    hovered = ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenOverlapped | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem);

    ImGui::SameLine(); ImGui::SetCursorPosX(cursor_pos_x);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0,0});
    ImGui::BeginColumns(nullptr, 2, ImGuiColumnsFlags_NoBorder | ImGuiColumnsFlags_NoResize);
    ImGui::SetColumnWidth(0, column_width);
    ImGui::PopStyleVar();

    if (accept_drop) {
        bool small_target_hovered = false;
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - spacing);
        if (const ImGuiPayload* payload = gui::DragDropTarget(ptr_id, DRAG_DROP_TYPE, {width, spacing}, small_target_hovered)) {
            uint32_t child_id = *(uint32_t *) payload->Data;
            auto *parent = comp->GetParent();
            ecs::entity_t parent_id = parent ? parent->GetID() : ecs::null;
            commands.emplace_back(SetParentCommand{child_id, parent_id, id });
        }
        ImGui::SameLine();
        ImGui::SetCursorPos({cursor_pos_x, ImGui::GetCursorPosY() + spacing});

        if (!small_target_hovered) {
            if (const ImGuiPayload *payload = gui::DragDropTarget(ptr_id, DRAG_DROP_TYPE, {width, height})) {
                uint32_t child_id = *(uint32_t *) payload->Data;
                commands.emplace_back(SetParentCommand{child_id, id, ecs::null});
            }
            ImGui::SameLine(); ImGui::SetCursorPosX(cursor_pos_x);
        }
    }

    ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_OpenOnArrow;
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

    ImGui::NextColumn();

    // Add Child button
    if (hovered) {
        auto btn_flags = ImGuiButtonFlags_PressedOnClick | ImGuiButtonFlags_AlignTextBaseLine |
            ImGuiButtonFlags_NoHoldingActiveId | ImGuiButtonFlags_NoHoveredOnFocus | ImGuiButtonFlags_AllowItemOverlap;

        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {0,0});
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0,0});
        if (ImGui::ButtonEx("+", ImVec2(15, 0), btn_flags)) {
            gui::SetItemOpen(ptr_id, true);
            commands.emplace_back(CreateEntityCommand{id});
        }
        ImGui::PopStyleVar(2);
    }

    ImGui::EndColumns();

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

std::unique_ptr<SceneGraphEditorGui> SceneGraphEditorGui::Create(Engine& e) {
    return std::make_unique<SceneGraphEditorGui>(e);
}

static void Draw(meta::Reference& instance, const meta::Field& field) {
    meta::Type type(field.GetType());
    meta::Reference ref(field.GetReference(instance));

    gui::PushID(field.GetName().c_str());

    if (type.Is<int>() ||
        type.Is<uint32_t>() ||
        type.Is<size_t>()) {
        auto& value = ref.Get<int>();
        gui::InputInt(field.GetName().c_str(), &value);
    } else if (type.Is<float>()) {
        auto& value = ref.Get<float>();
        gui::InputFloat(field.GetName().c_str(), &value);
    } else if (type.Is<double>()) {
        auto& value = ref.Get<double>();
        gui::InputDouble(field.GetName().c_str(), &value);
    } else if (type.Is<bool>()) {
        auto& value = ref.Get<bool>();
        gui::Checkbox(field.GetName().c_str(), &value);
    } else {
        gui::Text("%s: ", field.GetName().c_str());
        gui::Indent(20.0f);
        for (const auto& inner_field : type.GetFields()) {
            Draw(ref, inner_field);
        }
        gui::Unindent(20.0f);
    }

    gui::PopID();

}

void SceneGraphEditorGui::OnGui() {
    ImGui::Begin("Scene Graph");
    DrawTreeRecursive(engine_.get_scene()->GetRoots(), true, selected_, commands_);
    ImGui::End();

    ImGui::Begin("Test Properties");
    if (selected_ != ecs::null) {
        Scene *scene = engine_.get_scene();
        scene->visit(selected_, [&] (const meta::TypeId id) {
            meta::Type type(id);

            if (type.Is<TransformComponent>()) {
                meta::Reference ref(scene->get<TransformComponent>(selected_));
                gui::Text("Type: %s (%d)", type.Name().c_str(), id);

                for (const auto &field : type.GetFields()) {
                    Draw(ref, field);
                }
            }
        });
    }
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