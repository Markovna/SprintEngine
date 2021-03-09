#include "application.h"

int main() {
    std::filesystem::path path(argv[0]);
    std::filesystem::current_path(path.parent_path());

    sprint::Application app;
    return app.Run();
}
