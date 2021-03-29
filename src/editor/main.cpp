#include "editor_app.h"
#include "reflection/meta_module.h"
#include "core/serializer.h"
#include "core/components/camera.h"
#include "common/guid.h"

int main(int argc, char *argv[]) {
    meta::InitTypes();


//    std::stringstream ss;
//    sprint::Camera cam;
//    cam.SetFieldOfView(90.0f);
//    cam.SetClearColor(sprint::Color::Red);
//    cam.SetFar(3443);
//    cam.SetNear(42);
//    cam.SetRect({0.1f, 0.2f, 0.82f, 0.96f});
//
//    sprint::serializer::Serialize(ss, cam);
//
//    sprint::Camera cam2;
//    sprint::serializer::Deserialize(ss, cam2);
//
//    std::stringstream ss2;
//    sprint::serializer::Serialize(ss2, cam2);
//
//    std::cout << ss2.str();
//
//    return 0;

    sprint::editor::Application app(argv[1]);
    return app.Run();
}
