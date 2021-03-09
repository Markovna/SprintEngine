#include "math/matrix.h"
#include "engine.h"
#include "common/ecs/ecs.h"
#include "components/camera.h"
#include "components/mesh_renderer.h"
#include "input_events.h"

#include <GLFW/glfw3.h>

namespace sprint {

void Engine::Update() {
    SPRINT_PROFILE_FUNCTION();

    delta_time_ = timer_.Restart();

    {
        float timeValue = 0.5f * (float) glfwGetTime();
        for (ecs::entity_t ent : scene_->view<MeshRenderer>()) {
            auto &tr = scene_->get<TransformComponent>(ent);

//            Matrix model = Matrix::Identity;
//            model *= Matrix::Rotation(Quat(Vec3::Forward, 1.0f * timeValue));
//            model *= Matrix::Rotation(Quat(Vec3::Left, 1.25f * timeValue));
//            model *= Matrix::Translation(tr.GetLocalTransform().get_position());

//            tr.SetLocalTransform(Transform(model));
        }
    }
}

std::unique_ptr<Engine> Engine::Create() {
    return std::make_unique<Engine>();
}

Engine::Engine() {
    scene_ = Scene::Create();

    {
        auto root_ent = scene_->CreateEntity();
        auto& ent0_tr = scene_->get<TransformComponent>(root_ent);

        auto mesh_ent = scene_->CreateEntity({}, &scene_->get<TransformComponent>(root_ent));
        auto cam_ent = scene_->CreateEntity({});


//        auto ent3 = scene_->CreateEntity({}, &scene_->get<TransformComponent>(root_ent));

//        scene_->get<TransformComponent>(mesh_ent).SetParent(&scene_->get<TransformComponent>(root_ent), &scene_->get<TransformComponent>(ent3));
//        scene_->get<TransformComponent>(cam_ent).SetParent(&scene_->get<TransformComponent>(root_ent));
        scene_->emplace<MeshRenderer>(mesh_ent);

        for (int i = 1; i < 10; i++) {
            {
                vec3 pos = (i * 2.0f + 1) * Vec3::Forward + (i * 0.7f) * Vec3::Right;
                auto ent = scene_->CreateEntity({}, &scene_->get<TransformComponent>(root_ent));
                scene_->emplace<MeshRenderer>(ent);
                TransformComponent& tr = scene_->get<TransformComponent>(ent);
                tr.SetLocalTransform((Transform) Matrix::Translation(pos));
            }
            {
                vec3 pos = (i * 2.0f + 1) * Vec3::Forward + (i * 0.7f) * Vec3::Left;
                auto ent = scene_->CreateEntity({}, &scene_->get<TransformComponent>(root_ent));
                scene_->emplace<MeshRenderer>(ent);
                TransformComponent& tr = scene_->get<TransformComponent>(ent);
                tr.SetLocalTransform((Transform) Matrix::Translation(pos));
            }
        }

        auto& cam = scene_->emplace<Camera>(cam_ent);
        auto& cam_tr = scene_->get<TransformComponent>(cam_ent);
        cam_tr.SetLocalTransform(Transform(Matrix::Translation({0,-2.0f,-3.0f})));

//        input_events::OnKeyPress.connect([](KeyPressEvent& key){
//            if (key.key_code == key::Left || key.key_code == key::Right) {
//                float a = key.key_code == key::Left ? -1 : 1;
//                view_mat = Matrix::Rotation(Quat(Vec3::Right, a * 0.01f * M_PI)) * view_mat;
//                gfx::SetView(0, Matrix::GetInverse(view_mat));
//            }
//
//            else if (key.key_code == key::Up || key.key_code == key::Down || key.key_code == key::W || key.key_code == key::S) {
//                view_mat = Matrix::Translation((key.key_code == key::Up || key.key_code == key::W ? 0.1f : -0.1f) * Vec3::Up) * view_mat;
//                gfx::SetView(0, Matrix::GetInverse(view_mat));
//            }
//
//            else if (key.key_code == key::A || key.key_code == key::D) {
//                view_mat = Matrix::Translation((key.key_code == key::A ? -0.1f : 0.1f) * Vec3::Forward) * view_mat;
//                gfx::SetView(0, Matrix::GetInverse(view_mat));
//            }
//        });
    }
}

Engine::~Engine() {
}

TimeSpan Engine::get_delta() const {
    return delta_time_;
}

Scene* Engine::get_scene() {
    return scene_.get();
}

}