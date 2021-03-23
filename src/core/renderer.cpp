#include "renderer.h"
#include "engine.h"
#include "components/mesh_renderer.h"
#include "components/camera.h"

namespace sprint {

Matrix GetProjectionMatrix(const Camera& camera, const float ratio) {
    const Rect normalized_rect = camera.GetRect();
    const float fov = camera.GetFieldOfView();
    const float near = camera.GetNear();
    const float far = camera.GetFar();
    if (fov > 0.0f) {
        return Matrix::Perspective(
            fov * math::DEG_TO_RAD, ratio * normalized_rect.width / normalized_rect.height, near, far
            );
    }

    const float ortho_size = camera.GetOrthoSize();
    return Matrix::Ortho(
        ratio * ortho_size * normalized_rect.width,
        ortho_size * normalized_rect.height,
        near,
        far);
}

void Renderer::Render(Rect viewport, gfx::framebuf_handle fb_handle, Camera::Kind camera_kind) {
    auto mesh_view = scene_.view<MeshRenderer>();
    auto camera_view = scene_.view<Camera>();

    for (ecs::entity_t cam_entity : camera_view) {
        const Camera& camera = camera_view.get(cam_entity);
        if (camera.GetKind() != camera_kind)
            continue;

        const TransformComponent& camera_transform = scene_.get<TransformComponent>(cam_entity);
        Rect normalized_rect = camera.GetRect();

        gfx::CameraId camera_id = camera.CameraID();
        gfx::SetView(camera_id, Matrix::GetInverse((Matrix)camera_transform.GetTransform()));
        gfx::SetProjection(camera_id, GetProjectionMatrix(camera, viewport.width / viewport.height));
        gfx::SetViewRect(camera_id,{ 0, 0, int(viewport.width * normalized_rect.width), int(viewport.height * normalized_rect.height)});
        gfx::SetViewBuffer(camera_id, fb_handle);
        gfx::SetClear(camera_id, camera.GetClearFlag());
        gfx::SetClearColor(camera_id, camera.GetClearColor());

        for (ecs::entity_t entity : mesh_view) {

            const MeshRenderer& mesh = mesh_view.get(entity);
            const TransformComponent& transform = scene_.get<TransformComponent>(entity);
            const Transform& model = transform.GetTransform();

            gfx::SetUniform(mesh.GetMainTextureUniform(), mesh.GetMainTexture()->get_handle(), 0);
            gfx::SetUniform(mesh.GetSecondTextureUniform(), mesh.GetSecondTexture()->get_handle(), 1);
            gfx::SetUniform(mesh.GetColorUniform(), Color::White);

            gfx::SetTransform((Matrix) model);
            gfx::SetBuffer(mesh.GetVertexBuf());
            gfx::SetBuffer(mesh.GetIndexBuf());

            gfx::Render(camera_id, mesh.GetShader()->get_handle());
        }
    }
}

std::unique_ptr<Renderer> Renderer::Create(World &scene) {
    return std::make_unique<Renderer>(scene);
}

}