#pragma once

#include "window_event.h"
#include "window.h"
#include "signals/event.h"

#include "../editor/imgui_renderer.h"
#include "engine.h"

namespace sprint::input_events {

extern event<MouseDownEvent&> OnMouseDown;
extern event<MouseUpEvent&> OnMouseUp;
extern event<MouseMoveEvent&> OnMouseMove;
extern event<ScrollEvent&> OnScroll;
extern event<KeyPressEvent&> OnKeyPress;
extern event<KeyReleaseEvent&> OnKeyRelease;
extern event<TextEvent&> OnTextInput;
extern event<CloseEvent&> OnClose;
extern event<ResizeEvent&> OnResize;

void OnEvent(WindowEvent &event);

};


