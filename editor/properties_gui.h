#pragma once

#include <memory>

namespace sprint::editor {

class PropertiesEditorGui {
public:
    std::unique_ptr<PropertiesEditorGui> Create();

    PropertiesEditorGui() = default;
    ~PropertiesEditorGui() = default;
};

}
