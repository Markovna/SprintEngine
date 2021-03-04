#include "editor_gui.h"
#include "gui.h"
#include "scene_graph_gui.h"
#include "properties_gui.h"
#include "scene_view_gui.h"
#include "game_view_gui.h"

namespace sprint::editor {

static void BeginDockSpace() {
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->GetWorkPos());
    ImGui::SetNextWindowSize(viewport->GetWorkSize());
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(3);

    ImGuiIO& io = ImGui::GetIO();
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::MenuItem("Close", NULL, false, true)) {}
            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

std::unique_ptr<EditorGui> EditorGui::Create(Engine& engine) {
    return std::make_unique<EditorGui>(engine);
}

EditorGui::EditorGui(Engine& engine)
    : engine_(engine) {

    game_view_gui = GameViewGui::Create(*this, engine_);
    scene_graph_gui_ = SceneGraphEditorGui::Create(*this, engine_);
    properties_gui_ = PropertiesEditorGui::Create(*this, engine_);
    scene_view_gui_ = SceneViewGui::Create(*this, engine_);
}


void EditorGui::OnGui() {

    BeginDockSpace();

    game_view_gui->OnGui();
    scene_graph_gui_->OnGui();
    properties_gui_->OnGui();
    scene_view_gui_->OnGui();
}

EditorGui::~EditorGui() {

}

}