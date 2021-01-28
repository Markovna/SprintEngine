#include "input_events.h"
#include "window.h"
#include "log.h"

namespace sprint::input_events {

event<MouseDownEvent&> OnMouseDown;
event<MouseUpEvent&> OnMouseUp;
event<MouseMoveEvent&> OnMouseMove;
event<ScrollEvent&> OnScroll;
event<KeyPressEvent&> OnKeyPress;
event<KeyReleaseEvent&> OnKeyRelease;
event<TextEvent&> OnTextInput;
event<CloseEvent&> OnClose;
event<ResizeEvent&> OnResize;

template<class T> void OnEvent(T&);

template<> void OnEvent(MouseDownEvent& e) { OnMouseDown(e); }
template<> void OnEvent(MouseUpEvent& e) { OnMouseUp(e); }
template<> void OnEvent(MouseMoveEvent& e) { OnMouseMove(e); }
template<> void OnEvent(KeyPressEvent& e) { OnKeyPress(e); }
template<> void OnEvent(KeyReleaseEvent& e) { OnKeyRelease(e); }
template<> void OnEvent(ScrollEvent& e) { OnScroll(e); }
template<> void OnEvent(TextEvent& e) { OnTextInput(e); }
template<> void OnEvent(CloseEvent& e) { OnClose(e); }
template<> void OnEvent(ResizeEvent& e) { OnResize(e); }

void OnEvent(WindowEvent& event) {
    std::visit([](auto& e) { OnEvent(e); }, event);
}

}