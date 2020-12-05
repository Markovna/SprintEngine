#pragma once

#include <string>
#include <variant>

#include "KeyCode.h"
#include "MouseCodes.h"
#include "Vector.h"

namespace sprint {

struct MouseMoveEvent {
    Vec2 position;
};

struct MouseEvent {
    MouseCode mouse_code;
};

struct ScrollEvent {
    Vec2 offset;
};

struct KeyEvent {
    KeyCode key_code;
    bool control;
    bool shift;
    bool alt;
    bool super;
};

struct TextEvent {
    uint32_t unicode;
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
        SCROLL,
        KEY_PRESS,
        KEY_RELEASE,
        TEXT,
        CLOSE
    };

public:
    WindowEvent(KeyEvent key_event, bool pressed);
    WindowEvent(MouseEvent mouse_event, bool down);
    WindowEvent(MouseMoveEvent move_event);
    WindowEvent(CloseEvent close_event);
    WindowEvent(ScrollEvent scroll_event);
    WindowEvent(TextEvent text_event);
    WindowEvent();

    inline Type get_type() const;

private:
    Type type_;

public:
    union {
        MouseEvent mouse_event;
        MouseMoveEvent mouse_move_event;
        ScrollEvent scroll_event;
        KeyEvent key_event;
        TextEvent text_event;
        CloseEvent close_event;
        NoneEvent none;
    };
};

inline WindowEvent::Type WindowEvent::get_type() const { return type_; }

}