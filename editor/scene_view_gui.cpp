#include "vector.h"
#include "scene_view_gui.h"
#include "gui.h"
#include "engine.h"
#include "render_texture.h"
#include "renderer.h"
#include "scene.h"
#include "components/camera.h"
#include "key_codes.h"

namespace sprint::editor {

std::unique_ptr<SceneViewGui> SceneViewGui::Create(EditorGui& editor, Engine& engine) {
    return std::make_unique<SceneViewGui>(editor, engine);
}

SceneViewGui::SceneViewGui(EditorGui &editor, Engine &engine)
    : editor_(editor), engine_(engine), renderer_(Renderer::Create(*engine_.get_scene())) {

    Scene* scene = engine.get_scene();
    camera_ = scene->CreateEntity({});
    scene->emplace<Camera>(camera_, Camera::Editor);
    TransformComponent& transform = engine_.get_scene()->get<TransformComponent>(camera_);
    transform.SetLocalRotation(quat(vec3::Right, 30 * math::DEG_TO_RAD));
}

void SceneViewGui::OnGui() {
    ImGuiIO& io = gui::GetIO();

    gui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    gui::Begin("Scene View");
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

    renderer_->Render({0,0, (float) resolution.x, (float) resolution.y}, render_texture_->get_handle(), Camera::Editor);

    gui::Image((ImTextureID)(intptr_t)render_texture_->get_texture().get_handle().id, size, {0,1}, {1, 0});

    // --------------- Camera Navigation ---------------

    if (gui::IsWindowHovered() && io.MouseWheel != 0.0f) {
        ZoomCamera(io.MouseWheel);
    }

    if (gui::IsWindowFocused()) {
        vec2 move_delta;
        vec2 rotate_delta;

        bool moving = gui::IsKeyDown(key::LeftAlt) && gui::IsKeyDown(key::LeftSuper);
        bool rotating = !moving && gui::IsKeyDown(key::LeftAlt);
        if (moving) {
            auto mouse_delta = gui::GetMouseDragDelta(ImGuiMouseButton_Left);
            gui::ResetMouseDragDelta(ImGuiMouseButton_Left);
            move_delta.x = -0.1f * mouse_delta.x;
            move_delta.y = 0.1f * mouse_delta.y;
        }
        else if (rotating) {
            auto mouse_delta = gui::GetMouseDragDelta(ImGuiMouseButton_Left);
            gui::ResetMouseDragDelta(ImGuiMouseButton_Left);
            rotate_delta.x = mouse_delta.x;
            rotate_delta.y = mouse_delta.y;
        }
        else {
            if (gui::IsKeyDown(key::Up)) move_delta.y += 1;
            if (gui::IsKeyDown(key::Down)) move_delta.y -= 1;
            if (gui::IsKeyDown(key::Right)) move_delta.x += 1;
            if (gui::IsKeyDown(key::Left)) move_delta.x -= 1;
        }

        if (move_delta != vec2::Zero)
            MoveCamera(move_delta);

        if (rotate_delta != vec2::Zero)
            RotateCamera(rotate_delta);
    }
    // -------------------------------------------------

    gui::End();
}

SceneViewGui::~SceneViewGui() {

}

void SceneViewGui::MoveCamera(const vec2 &delta) {
    static const float speed = 0.2f;
    TransformComponent& transform = engine_.get_scene()->get<TransformComponent>(camera_);
    vec3 direction = transform.GetLocalTransform().TransformDirection((speed * vec3(delta)));
    transform.SetLocalPosition(transform.GetLocalPosition() + direction);
}

void SceneViewGui::ZoomCamera(float delta) {
    static const float speed = 1.0f;
    TransformComponent& transform = engine_.get_scene()->get<TransformComponent>(camera_);
    vec3 direction = transform.GetLocalTransform().TransformDirection((speed * delta * vec3::Forward));
    transform.SetLocalPosition(transform.GetLocalPosition() + direction);
}

void SceneViewGui::RotateCamera(const vec2 &delta) {
    static const float distance = 2.0f;
    static const float speed = 0.1f;
    TransformComponent& transform = engine_.get_scene()->get<TransformComponent>(camera_);
    const vec3 up = transform.GetLocalTransform().Up();
    const vec3 right = transform.GetLocalTransform().Right();
    const vec3 fwd = transform.GetLocalTransform().Forward();

    vec3 dir = distance * fwd;
    quat rot(vec3::Up, speed * delta.x * math::DEG_TO_RAD);
    rot *= quat(right, speed * delta.y * math::DEG_TO_RAD);

    vec3 center = transform.GetLocalPosition() + dir;
    vec3 local_pos = center + rot * -dir;

    transform.SetLocalPosition(local_pos);
    transform.SetLocalRotation(quat::LookAt(center - local_pos, rot * up));
}

}