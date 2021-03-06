#include "scene_view_gui.h"
#include "gui.h"

namespace sprint::editor {

std::unique_ptr<SceneViewGui> SceneViewGui::Create(EditorGui& editor, Engine& engine) {
    return std::make_unique<SceneViewGui>(editor, engine);
}

void SceneViewGui::OnGui() {

    gui::Begin("Scene View");



    gui::End();
}

}