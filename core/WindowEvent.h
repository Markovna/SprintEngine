#pragma once

#include <string>
#include <variant>

#include "KeyCode.h"
#include "MouseCodes.h"
#include "Vector.h"

namespace sprint {

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
    WindowEvent(KeyEvent key_event, bool pressed);
    WindowEvent(MouseEvent mouse_event, bool down);
    WindowEvent(MouseMoveEvent move_event);
    WindowEvent(CloseEvent close_event);
    WindowEvent();

    inline Type get_type() const;

private:
    Type type_;

public:
    union {
        MouseEvent Mouse;
        MouseMoveEvent MouseMove;
        KeyEvent Key;
        CloseEvent Close;
        NoneEvent None;
    };
};

inline WindowEvent::Type WindowEvent::get_type() const { return type_; }

}