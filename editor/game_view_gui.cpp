#include "game_view_gui.h"
#include "gui.h"
#include "renderer.h"
#include "texture.h"
#include "engine.h"

namespace sprint::editor {

std::unique_ptr<GameViewGui> GameViewGui::Create(EditorGui& editor, Engine& engine) {
    return std::make_unique<GameViewGui>(editor, engine);
}

GameViewGui::GameViewGui(EditorGui &editor, Engine &engine)
    : editor_(editor), engine_(engine) {
    renderer_ = Renderer::Create(*engine_.get_scene());
}

void GameViewGui::OnGui() {
    gui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    gui::Begin("Game View");
    gui::PopStyleVar();

    ImVec2 scale = gui::GetIO().DisplayFramebufferScale;
    ImVec2 size = { gui::GetWindowWidth(), gui::GetContentRegionAvail().y };
    Vec2Int resolution(int(size.x * scale.x), int(size.y * scale.y));

    if (!render_texture_ ||
        resolution.x != render_texture_->get_texture().get_width() ||
        resolution.y != render_texture_->get_texture().get_height()) {

        render_texture_.reset();
        render_texture_ = std::make_unique<RenderTexture>(resolution.x, resolution.y, gfx::TextureFormat::RGBA8, gfx::TextureWrap{}, gfx::TextureFilter{}, gfx::TextureFlags::None);
    }

    renderer_->Render({0,0, (float) resolution.x, (float) resolution.y}, render_texture_->get_handle());

    gui::Image((ImTextureID)(intptr_t)render_texture_->get_texture().get_handle().id, size, {0,1}, {1, 0});

    gui::End();
}

GameViewGui::~GameViewGui() {

}

}