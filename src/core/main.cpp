#include "application.h"
#include <filesystem>

int main(int argc, char *argv[]) {
//    std::filesystem::path path(argv[0]);
//    std::filesystem::current_path(path.parent_path());

    sprint::Application app;
    return app.Run();
}
