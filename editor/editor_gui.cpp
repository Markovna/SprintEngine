#include "imgui.h"
#include "imgui_internal.h"

#include "texture.h"
#include "editor_gui.h"
#include "signals/delegate.h"
#include "signals/event.h"
#include "gui.h"

namespace sprint::editor {

//TODO
static std::unique_ptr<Texture> render_tex;
static gfx::texture_handle depth_tex;
static gfx::framebuf_handle frame_buffer;
static void TestGui(float delta) {

    static bool show = true;
    ImGui::ShowDemoWindow(&show);
    ImGui::Text("%s", log::Format("fps={0:.5f}", 1.0f / delta).c_str());
}

std::unique_ptr<EditorGui> EditorGui::Create(Window& window, Engine& engine) {
    return std::make_unique<EditorGui>(window, engine);
}

EditorGui::EditorGui(Window& window, Engine& engine)
    : window_(window), engine_(engine) {

    scene_graph_gui_ = SceneGraphEditorGui::Create(engine_);
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

        engine_.SetOutput(frame_buffer);
    }
}

EditorGui::~EditorGui() {

}


void EditorGui::OnGui() {

    // TODO
    {
        TestGui(engine_.get_delta().AsSeconds());
        float width = ImGui::GetWindowWidth();
        float height = render_tex->get_height() * (width / render_tex->get_width());
        ImGui::Image((ImTextureID)(intptr_t)render_tex->get_handle().id, {width, height}, {0,1}, {1, 0});
    }

    scene_graph_gui_->OnGui();
}

}