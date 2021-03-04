#include "renderer.h"
#include "engine.h"
#include "components/mesh_renderer.h"
#include "components/camera.h"

namespace sprint {

Matrix GetProjectionMatrix(const Camera& camera, const Rect& viewport) {
    const Rect normalized_rect = camera.GetRect();
    const float fov = camera.GetFieldOfView();
    const float near = camera.GetNear();
    const float far = camera.GetFar();
    const float ratio = viewport.width / viewport.height;
    if (fov > 0.0f) {
        return Matrix::Perspective(
            fov * math::PI / 180.0f, ratio * normalized_rect.width / normalized_rect.height, near, far
            );
    }

    const float ortho_size = camera.GetOrthoSize();
    return Matrix::Ortho(
        ratio * ortho_size * normalized_rect.width,
        ortho_size * normalized_rect.height,
        near,
        far);
}

void Renderer::Render(Rect viewport, gfx::framebuf_handle fb_handle) {
    auto mesh_view = scene_.view<MeshRenderer>();
    auto camera_view = scene_.view<Camera>();

    for (ecs::entity_t cam_entity : camera_view) {
        const Camera& camera = scene_.get<Camera>(cam_entity);
        const TransformComponent& camera_transform = scene_.get<TransformComponent>(cam_entity);
        Rect normalized_rect = camera.GetRect();

        gfx::SetView(camera.CameraID(), Matrix::GetInverse((Matrix)camera_transform.GetTransform()));
        gfx::SetProjection(camera.CameraID(), GetProjectionMatrix(camera, viewport));
        gfx::SetViewRect(camera.CameraID(),
                         {
            0,
            0,
            (uint32_t)(viewport.width * normalized_rect.width),
            (uint32_t)(viewport.height * normalized_rect.height)});
        gfx::SetViewBuffer(camera.CameraID(), fb_handle);

        gfx::SetClear(0, gfx::ClearFlag::Color | gfx::ClearFlag::Depth);
        gfx::SetClearColor(0, Color(0.1, 0.1, 0.1, 1.0));

        for (ecs::entity_t entity : mesh_view) {

            const MeshRenderer& mesh = scene_.get<MeshRenderer>(entity);
            const TransformComponent& transform = scene_.get<TransformComponent>(entity);
            const Transform& model = transform.GetTransform();

            gfx::SetUniform(mesh.GetMainTextureUniform(), mesh.GetMainTexture()->get_handle(), 0);
            gfx::SetUniform(mesh.GetSecondTextureUniform(), mesh.GetSecondTexture()->get_handle(), 1);
            gfx::SetUniform(mesh.GetColorUniform(), Color::White);

            gfx::SetTransform((Matrix) model);
            gfx::SetBuffer(mesh.GetVertexBuf());
            gfx::SetBuffer(mesh.GetIndexBuf());

            gfx::Render(camera.CameraID(), mesh.GetShader()->get_handle());
        }
    }
}
std::unique_ptr<Renderer> Renderer::Create(Scene &scene) {
    return std::make_unique<Renderer>(scene);
}

}