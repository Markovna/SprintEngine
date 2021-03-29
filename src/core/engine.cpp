#include "math/matrix.h"
#include "engine.h"
#include "common/ecs/ecs.h"
#include "components/camera.h"
#include "components/mesh_renderer.h"
#include "input_events.h"
#include "scene.h"

#include <GLFW/glfw3.h>

namespace sprint {

void Engine::Update() {
    SPRINT_PROFILE_FUNCTION();

    delta_time_ = timer_.Restart();

    {
        float timeValue = 0.5f * (float) glfwGetTime();
        for (ecs::entity_t ent : world_->View<MeshRenderer>()) {
            auto &tr = world_->Get<TransformComponent>({ent});

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

    world_ = World::Create();

    {
        auto root_ent = world_->CreateEntity();
        auto& ent0_tr = world_->Get<TransformComponent>(root_ent);

        auto mesh_ent = world_->CreateEntity({}, root_ent);
        auto cam_ent = world_->CreateEntity({});

        world_->AddComponent<MeshRenderer>(mesh_ent);

        for (int i = 1; i < 10; i++) {
            {
                vec3 pos = (i * 2.0f + 1) * Vec3::Forward + (i * 0.7f) * Vec3::Right;
                auto ent = world_->CreateEntity({}, root_ent);
                world_->AddComponent<MeshRenderer>(ent);
                world_->SetLocalTransform(ent, (Transform) Matrix::Translation(pos));
            }
            {
                vec3 pos = (i * 2.0f + 1) * Vec3::Forward + (i * 0.7f) * Vec3::Left;
                auto ent = world_->CreateEntity({}, root_ent);
                world_->AddComponent<MeshRenderer>(ent);
                world_->SetLocalTransform(ent, (Transform) Matrix::Translation(pos));
            }
        }

        auto& cam = world_->AddComponent<Camera>(cam_ent);
        world_->SetLocalTransform(cam_ent, Transform(Matrix::Translation({0,-2.0f,-3.0f})));

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

World* Engine::get_world() {
    return world_.get();
}

}