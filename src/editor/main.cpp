#include "editor_app.h"
#include "reflection/meta_module.h"

int main(int argc, char *argv[]) {
    meta::InitTypes();

    sprint::editor::Application app(argv[1]);
    return app.Run();
}
