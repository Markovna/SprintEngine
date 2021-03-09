#include "editor_app.h"
#include "../reflection/meta_module.h"
#include <filesystem>

int main(int argc, char *argv[]) {
    meta::InitTypes();

    sprint::editor::Application app(sprint::editor::Application::Config::Parse(argc, argv));
    return app.Run();
}
