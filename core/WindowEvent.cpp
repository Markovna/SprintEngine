#include "WindowEvent.h"


namespace Sprint {

WindowEvent::WindowEvent(KeyEvent keyEvent, bool pressed) :
        Key(std::move(keyEvent)),
        m_Type(pressed ? Type::KEY_PRESS : Type::KEY_RELEASE) {}

WindowEvent::WindowEvent(MouseEvent mouseEvent, bool down) :
        Mouse(std::move(mouseEvent)),
        m_Type(down ? Type::MOUSE_DOWN : Type::MOUSE_UP) {}

WindowEvent::WindowEvent(CloseEvent closeEvent) :
        Close(closeEvent), m_Type(Type::CLOSE) {}

WindowEvent::WindowEvent(MouseMoveEvent mouseEvent) :
        MouseMove(std::move(mouseEvent)),
        m_Type(Type::MOUSE_MOVE) {}

WindowEvent::WindowEvent() :
        None(NoneEvent()), m_Type(Type::NONE) {}

}