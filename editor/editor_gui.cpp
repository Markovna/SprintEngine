#include "gui.h"

#include "texture.h"
#include "editor_gui.h"
#include "window.h"
#include "renderer.h"
#include "scene_graph_gui.h"
#include "properties_gui.h"
#include "scene_view_gui.h"

namespace sprint::editor {

//TODO
static std::unique_ptr<Texture> render_tex;
static gfx::texture_handle depth_tex;
static gfx::framebuf_handle frame_buffer;

static void Begin() {
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

std::unique_ptr<EditorGui> EditorGui::Create(Window& window, Engine& engine) {
    return std::make_unique<EditorGui>(window, engine);
}

EditorGui::EditorGui(Window& window, Engine& engine)
    : window_(window), engine_(engine) {

    scene_graph_gui_ = SceneGraphEditorGui::Create(*this, engine_);
    properties_gui_ = PropertiesEditorGui::Create(*this, engine_);
    scene_view_gui_ = SceneViewGui::Create(*this, engine_);

    // TODO
    {
        render_tex = std::make_unique<Texture>(Texture{
            gfx::MemoryPtr{}, (uint32_t) window_.get_resolution().x, (uint32_t) window_.get_resolution().y,
            gfx::TextureFormat::RGBA8, gfx::TextureWrap{}, gfx::TextureFilter{}, gfx::TextureFlags::None});
        depth_tex = gfx::CreateTexture((uint32_t) window_.get_resolution().x,
                                       (uint32_t) window_.get_resolution().y,
                                       gfx::TextureFormat::D24S8,
                                       {},
                                       {},
                                       gfx::TextureFlags::RenderTarget,
                                       {});
        frame_buffer = gfx::CreateFrameBuffer({render_tex->get_handle(), depth_tex});

    }
    renderer_ = Renderer::Create(*engine_.get_scene(), frame_buffer);
}

EditorGui::~EditorGui() {

}


void EditorGui::OnGui() {

    Begin();

    // game view
    {
        gui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        gui::Begin("Game View");
        gui::PopStyleVar();

        renderer_->Render();

        float width = ImGui::GetWindowWidth();
        float height = render_tex->get_height() * (width / render_tex->get_width());
        gui::Image((ImTextureID)(intptr_t)render_tex->get_handle().id, {width, height}, {0,1}, {1, 0});

        gui::End();
    }

    scene_graph_gui_->OnGui();
    properties_gui_->OnGui();
    scene_view_gui_->OnGui();
}

}