#include "editor_app.h"
#include "../reflection/meta_module.h"

int main() {
    meta::InitTypes();

    sprint::editor::Application app;
    return app.Run();
}
