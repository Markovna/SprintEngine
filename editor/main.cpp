#include "editor_app.h"
#include "../reflection/meta_module.h"
#include <filesystem>

int main(int argc, char *argv[]) {
    std::filesystem::path path(argv[0]);
    std::filesystem::current_path(path.parent_path());

    meta::InitTypes();

    sprint::editor::Application app;
    return app.Run();
}
