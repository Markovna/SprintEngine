#include "WindowEvent.h"


namespace sprint {

WindowEvent::WindowEvent(KeyEvent key_event, bool pressed) :
        Key(std::move(key_event)),
        type_(pressed ? Type::KEY_PRESS : Type::KEY_RELEASE) {}

WindowEvent::WindowEvent(MouseEvent mouse_event, bool down) :
        Mouse(std::move(mouse_event)),
        type_(down ? Type::MOUSE_DOWN : Type::MOUSE_UP) {}

WindowEvent::WindowEvent(CloseEvent close_event) :
        Close(close_event), type_(Type::CLOSE) {}

WindowEvent::WindowEvent(MouseMoveEvent move_event) :
        MouseMove(std::move(move_event)),
        type_(Type::MOUSE_MOVE) {}

WindowEvent::WindowEvent() :
        None(NoneEvent()), type_(Type::NONE) {}

}