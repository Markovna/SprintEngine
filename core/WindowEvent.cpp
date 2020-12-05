#include "WindowEvent.h"


namespace sprint {

WindowEvent::WindowEvent(KeyEvent key_event, bool pressed) :
    key_event(key_event),
    type_(pressed ? Type::KEY_PRESS : Type::KEY_RELEASE) {}

WindowEvent::WindowEvent(MouseEvent mouse_event, bool down) :
    mouse_event(mouse_event),
    type_(down ? Type::MOUSE_DOWN : Type::MOUSE_UP) {}

WindowEvent::WindowEvent(CloseEvent close_event) :
    close_event(close_event), type_(Type::CLOSE) {}

WindowEvent::WindowEvent(MouseMoveEvent move_event) :
    mouse_move_event(move_event),
    type_(Type::MOUSE_MOVE) {}

WindowEvent::WindowEvent() :
    none(NoneEvent()), type_(Type::NONE) {}

WindowEvent::WindowEvent(ScrollEvent scroll_event) :
    scroll_event(scroll_event),
    type_(Type::SCROLL) {}

WindowEvent::WindowEvent(TextEvent text_event) :
    text_event(text_event),
    type_(Type::TEXT)
{}

}