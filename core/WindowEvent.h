#pragma once

#include <string>
#include <variant>

#include "KeyCode.h"
#include "MouseCodes.h"
#include "Vector.h"

namespace Sprint {

struct MouseMoveEvent {
    explicit MouseMoveEvent(Vec2 position) : Position(position) {}
    Vec2 Position;
};

struct MouseEvent {
    explicit MouseEvent(MouseCode mouseCode) : MouseCode(mouseCode) {}
    MouseCode MouseCode;
};

struct KeyEvent {
    explicit KeyEvent(KeyCode keyCode) : KeyCode(keyCode) {}
    KeyCode KeyCode;
};

struct CloseEvent {};

struct NoneEvent {};

class WindowEvent {
public:
    enum Type : uint8_t {
        NONE,
        MOUSE_UP,
        MOUSE_DOWN,
        MOUSE_MOVE,
        KEY_PRESS,
        KEY_RELEASE,
        CLOSE
    };

public:
    WindowEvent(KeyEvent keyEvent, bool pressed);
    WindowEvent(MouseEvent mouseEvent, bool down);
    WindowEvent(MouseMoveEvent mouseEvent);
    WindowEvent(CloseEvent closeEvent);
    WindowEvent();

    inline Type GetType() const;

private:
    Type m_Type;

public:
    union {
        MouseEvent Mouse;
        MouseMoveEvent MouseMove;
        KeyEvent Key;
        CloseEvent Close;
        NoneEvent None;
    };
};

inline WindowEvent::Type WindowEvent::GetType() const { return m_Type; }

}