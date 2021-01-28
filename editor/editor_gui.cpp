#include "editor_gui.h"

namespace sprint::editor {

std::unique_ptr<EditorGUI> EditorGUI::Create() {
    return std::unique_ptr<EditorGUI>();
}

EditorGUI::EditorGUI() {

}

EditorGUI::~EditorGUI() {

}

void EditorGUI::OnGui() {

}

}