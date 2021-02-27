#include "gfx.h"
#include "matrix.h"
#include "texture.h"
#include "shader.h"
#include "engine.h"
#include "log.h"
#include "sparse_set.h"
#include "ecs/ecs.h"
#include "input_events.h"
#include "components/mesh_component.h"

#include <GLFW/glfw3.h>

namespace sprint {

static std::unique_ptr<Shader> m_Shader;
static std::shared_ptr<Texture> m_Texture1;
static std::shared_ptr<Texture> m_Texture2;
static gfx::vertexbuf_handle vb_handle;
static gfx::indexbuf_handle ib_handle;
static gfx::uniform_handle texture_uniform_handle;
static gfx::uniform_handle texture2_uniform_handle;
static gfx::uniform_handle color_uniform_handle;
static Matrix view_mat;
static Matrix proj_mat;

static float* GetVertices() {
    static float vertices[] = {
        // pos                // tex coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    return vertices;
}
static uint32_t* GetIndices(size_t& count) {
    static uint32_t indices[] = {
        0,  1,  2,
        3,  4,  5,
        6,  7,  8,
        9,  10, 11,
        12, 13, 14,
        15, 16, 17,
        18, 19, 20,
        21, 22, 23,
        24, 25, 26,
        27, 28, 29,
        30, 31, 32,
        33, 34, 35
    };
    count = 36;
    return indices;
}
void TestSparseSet() {
    sparse_set<uint32_t> test_set;
    test_set.insert(0);
    test_set.insert(23);

    assert(test_set.contains(0));
    assert(test_set.contains(23));
    assert(!test_set.contains(12));

    test_set.erase(0);

    assert(!test_set.contains(0));
    assert(test_set.contains(23));

    test_set.insert(15);

    assert(!test_set.contains(0));
    assert(test_set.contains(23));
    assert(test_set.contains(15));
}
static void TestEcs() {
    TestSparseSet();
    ecs::registry set;
    auto entity1 = set.create();
    auto entity2 = set.create();
    auto entity3 = set.create();
    auto entity4 = set.create();

    assert(entity1 != entity2);
    assert(entity1 != entity3);
    assert(entity1 != entity4);
    assert(entity2 != entity3);
    assert(entity2 != entity4);
    assert(entity3 != entity4);

    assert(set.valid(entity1));
    assert(set.valid(entity2));
    assert(set.valid(entity3));
    assert(set.valid(entity4));

    set.destroy(entity2);
    assert(set.valid(entity1));
    assert(!set.valid(entity2));
    assert(set.valid(entity3));
    assert(set.valid(entity4));

    set.destroy(entity4);
    assert(set.valid(entity1));
    assert(!set.valid(entity2));
    assert(set.valid(entity3));
    assert(!set.valid(entity4));

    entity4 = set.create();
    assert(set.valid(entity1));
    assert(!set.valid(entity2));
    assert(set.valid(entity3));
    assert(set.valid(entity4));

    entity2 = set.create();
    assert(set.valid(entity1));
    assert(set.valid(entity2));
    assert(set.valid(entity3));
    assert(set.valid(entity4));


//    struct TransformComponent {
//        Vec3 positions;
//        Quat rotation;
//        Vec3 scale;
//    };
//
//    struct MeshComponent {
//        float value;
//    };
//
//    set.emplace<TransformComponent>(entity1, TransformComponent{Vec3::Zero, Quat::Identity, Vec3::One});
//    set.emplace<MeshComponent>(entity1, MeshComponent{1.0f});
//
//    set.emplace<TransformComponent>(entity2, TransformComponent{Vec3::Zero, Quat::Identity, Vec3::One});
//    set.emplace<MeshComponent>(entity2, MeshComponent{1.0f});
//
//    set.emplace<TransformComponent>(entity3, TransformComponent{Vec3::Zero, Quat::Identity, Vec3::One});
//    set.emplace<TransformComponent>(entity4, TransformComponent{Vec3::Zero, Quat::Identity, Vec3::One});
//
//    auto view = set.view<TransformComponent, MeshComponent>();
//    for (auto e : view) {
//        TransformComponent& tr = view.get<TransformComponent>(e);
//        auto [trans, mesh] = view.get<TransformComponent, MeshComponent>(e);
//        trans.scale = 2*Vec3::One;
//
//        auto [trans1, mesh1] = view.get(e);
//
//        log::core::Info("TransformComponent & MeshComponent entity: {}", e);
//    }

//    auto& tr = set.get<TransformComponent>(entity1);
//    assert(tr.scale == 2*Vec3::One);
//
//    auto transform_view = set.view<TransformComponent>();
//    for (auto e : transform_view) {
//        TransformComponent& tr = transform_view.get(e);
//        log::core::Info("TransformComponent entity: {}", e);
//    }

}

static void PrepareRenderTriangles(uint32_t width, uint32_t height) {

    TestEcs();

    m_Texture1  = Texture::Load("assets/textures/container.jpg");
    m_Texture2 = Texture::Load("assets/textures/seal.png");
    m_Shader   = std::make_unique<Shader>(Shader::Load(
        "assets/shaders/TestShader.shader",
        {
            gfx::Attribute::Binding::POSITION,
            gfx::Attribute::Binding::TEXCOORD0
        })
    );

    view_mat = Matrix::Translation(Vec3(0.0f, 0.0f, -3.0f));
    proj_mat = Matrix::Perspective(60.0f * M_PI / 180.0f, (float)width, (float)height, 0.01f, 100.0f);
//    proj_mat = Matrix::Ortho((float)width*0.05f, (float)height*0.05f, 0.1f, 100.0f);

    gfx::SetView(0, Matrix::GetInverse(view_mat));
    gfx::SetViewRect(0, {0, 0, width, height});
    gfx::SetProjection(0, proj_mat);
    gfx::SetClear(0, gfx::ClearFlag::Color | gfx::ClearFlag::Depth);
    gfx::SetClearColor(0, Color(0.1, 0.1, 0.1, 1.0));

    input_events::OnKeyPress.connect([](KeyPressEvent& key){
        if (key.key_code == key::Left || key.key_code == key::Right) {
            float a = key.key_code == key::Left ? -1 : 1;
            view_mat = Matrix::Rotation(Quat(Vec3::Right, a * 0.01f * M_PI)) * view_mat;
            gfx::SetView(0, Matrix::GetInverse(view_mat));
        }

        else if (key.key_code == key::Up || key.key_code == key::Down || key.key_code == key::W || key.key_code == key::S) {
            view_mat = Matrix::Translation((key.key_code == key::Up || key.key_code == key::W ? 0.1f : -0.1f) * Vec3::Up) * view_mat;
            gfx::SetView(0, Matrix::GetInverse(view_mat));
        }

        else if (key.key_code == key::A || key.key_code == key::D) {
            view_mat = Matrix::Translation((key.key_code == key::A ? -0.1f : 0.1f) * Vec3::Forward) * view_mat;
            gfx::SetView(0, Matrix::GetInverse(view_mat));
        }
    });

    texture_uniform_handle = gfx::CreateUniform("Texture1");
    texture2_uniform_handle = gfx::CreateUniform("Texture2");
    color_uniform_handle = gfx::CreateUniform("mainColor");

    gfx::SetUniform(texture_uniform_handle, m_Texture1->get_handle(), 0);
    gfx::SetUniform(texture2_uniform_handle, m_Texture2->get_handle(), 1);

    size_t count;
    float* vertices = GetVertices();
    uint32_t* indices = GetIndices(count);

    vb_handle = gfx::CreateVertexBuffer(
        gfx::MakeRef(vertices, sizeof(float) * 5 * count),
        count,
        {
            {gfx::Attribute::Binding::POSITION,  gfx::Attribute::Format::Vec3 },
            {gfx::Attribute::Binding::TEXCOORD0, gfx::Attribute::Format::Vec2 }
        }
    );

    ib_handle = gfx::CreateIndexBuffer(gfx::MakeRef(indices, sizeof(uint32_t) * count ), count);
}

static void RenderTriangle(float timeValue, Vec3 position) {

    Color color(sin(timeValue) / 2.0f + 0.5f,0.3f,cos(timeValue) / 2.0f + 0.5f,1.0f);

    Matrix model = Matrix::Identity;
    model *= Matrix::Rotation(Quat(Vec3::Forward, 1.0f * timeValue));
    model *= Matrix::Rotation(Quat(Vec3::Left, 1.25f * timeValue));
    model *= Matrix::Translation(position);

    gfx::SetUniform(texture_uniform_handle, m_Texture1->get_handle(), 0);
    gfx::SetUniform(texture2_uniform_handle, m_Texture2->get_handle(), 1);
    gfx::SetUniform(color_uniform_handle, color);

    gfx::SetTransform(model);
    gfx::SetBuffer(vb_handle);
    gfx::SetBuffer(ib_handle);

    gfx::Render(0, m_Shader->get_handle());
}

static void RenderTrianglezzz() {

    float timeValue =  0.5f * (float) glfwGetTime();

    const float radius = 10.0f;
    float camX = sin(timeValue) * radius;
    float camZ = cos(timeValue) * radius;

    RenderTriangle(timeValue, Vec3::Zero);
    RenderTriangle(timeValue, 2 * Vec3::Right);

    for (int i = 1; i < 10; i++) {
        RenderTriangle(timeValue + i * 0.1f, (i * 2.0f + 1) * Vec3::Up + (i * 0.7f) * Vec3::Forward);
        RenderTriangle(timeValue - i * 0.1f, (i * 2.0f + 1) * Vec3::Up + (i * 0.7f) * Vec3::Backward);
    }
}

static void ClearResources() {
    m_Texture1.reset();
    m_Texture2.reset();
    m_Shader.reset();

    gfx::Destroy(vb_handle);
    gfx::Destroy(ib_handle);
}

// -----------------------------------------------------------

void Engine::Update() {
    SPRINT_PROFILE_FUNCTION();

    delta_time_ = timer_.Restart();

    RenderTrianglezzz();
}

std::unique_ptr<Engine> Engine::Create(const Window& window) {
    return std::make_unique<Engine>(window);
}

Engine::Engine(const Window& window) {
    scene_ = Scene::Create();

    {
        auto ent0 = scene_->CreateEntity();
        auto& ent0_tr = scene_->get<TransformComponent>(ent0);

        auto ent1 = scene_->CreateEntity({}, &scene_->get<TransformComponent>(ent0));
        auto ent2 = scene_->CreateEntity({}, &scene_->get<TransformComponent>(ent0));
        auto ent3 = scene_->CreateEntity({}, &scene_->get<TransformComponent>(ent0));

        scene_->get<TransformComponent>(ent1).SetParent(&scene_->get<TransformComponent>(ent0), &scene_->get<TransformComponent>(ent3));
        scene_->get<TransformComponent>(ent2).SetParent(&scene_->get<TransformComponent>(ent0), &scene_->get<TransformComponent>(ent3));
        scene_->emplace<MeshComponent>(ent1);
    }

    if (false)
    {
        auto ent0 = scene_->CreateEntity();
        auto ent1 = scene_->CreateEntity();
        auto ent2 = scene_->CreateEntity();

        assert(scene_->get<TransformComponent>(ent0).GetChildrenSize() == 0);
        assert(scene_->get<TransformComponent>(ent1).GetChildrenSize() == 0);
        assert(scene_->get<TransformComponent>(ent2).GetChildrenSize() == 0);

//    auto& ent2_tr = scene_->get<TransformComponent>(ent2);
        auto ent3 = scene_->CreateEntity({}, &scene_->get<TransformComponent>(ent0));
        auto ent4 = scene_->CreateEntity({}, &scene_->get<TransformComponent>(ent1));

        assert(scene_->get<TransformComponent>(ent0).GetChildrenSize() == 1);
        assert(scene_->get<TransformComponent>(ent1).GetChildrenSize() == 1);

        auto ent5 = scene_->CreateEntity({}, &scene_->get<TransformComponent>(ent4));
        auto &tr_ent4 = scene_->get<TransformComponent>(ent4);
        tr_ent4.SetParent(&scene_->get<TransformComponent>(ent2));

        auto &tr_ent3 = scene_->get<TransformComponent>(ent3);
        tr_ent3.SetParent(nullptr);
    }

    PrepareRenderTriangles(window.get_resolution().x, window.get_resolution().y);
}

Engine::~Engine() {
    ClearResources();
}

void Engine::SetOutput(gfx::framebuf_handle handle) {
    gfx::SetViewBuffer(0, handle);
}

TimeSpan Engine::get_delta() const {
    return delta_time_;
}

Scene* Engine::get_scene() {
    return scene_.get();
}

}