#pragma once

#include <memory>

namespace sprint::editor {

class EditorGUI {
public:
    static std::unique_ptr<EditorGUI> Create();

    EditorGUI();
    ~EditorGUI();

    void OnGui();
};

}
