#include "scene.h"

namespace sprint {

std::unique_ptr<Scene> Scene::Create() {
    return std::make_unique<Scene>();
}

}