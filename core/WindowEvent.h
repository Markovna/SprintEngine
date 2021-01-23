#pragma once

#include <string>
#include <variant>

#include "KeyCode.h"
#include "MouseCodes.h"
#include "Vector.h"

namespace sprint {

enum EventType : uint8_t {
    MOUSE_UP,
    MOUSE_DOWN,
    MOUSE_MOVE,
    SCROLL,
    KEY_PRESS,
    KEY_RELEASE,
    TEXT,
    CLOSE,
    RESIZE
};

struct MouseMoveEvent {
    static EventType Type() { return EventType::MOUSE_MOVE; }
    Vec2 position;
};

struct MouseUpEvent {
    static EventType Type() { return EventType::MOUSE_UP; }
    MouseCode mouse_code;
};

struct MouseDownEvent {
    static EventType Type() { return EventType::MOUSE_DOWN; }
    MouseCode mouse_code;
};

struct ScrollEvent {
    static EventType Type() { return EventType::SCROLL; }
    Vec2 offset;
};

struct KeyPressEvent {
    static EventType Type() { return EventType::KEY_PRESS; }
    KeyCode key_code;
    bool control;
    bool shift;
    bool alt;
    bool super;
    bool repeat;
};

struct KeyReleaseEvent {
    static EventType Type() { return EventType::KEY_RELEASE; }
    KeyCode key_code;
    bool control;
    bool shift;
    bool alt;
    bool super;
};

struct TextEvent {
    static EventType Type() { return EventType::TEXT; }
    uint32_t unicode;
};

struct CloseEvent {
    static EventType Type() { return EventType::CLOSE; }
};

struct ResizeEvent {
    static EventType Type() { return EventType::RESIZE; }
};

using WindowEventVariant = std::variant<
    MouseMoveEvent,
    MouseUpEvent,
    MouseDownEvent,
    ScrollEvent,
    KeyPressEvent,
    KeyReleaseEvent,
    TextEvent,
    CloseEvent,
    ResizeEvent>;

class WindowEvent : public WindowEventVariant {
public:
    WindowEvent() : WindowEventVariant() {}

    template<class T>
    WindowEvent(T&& event) : WindowEventVariant(std::forward<T>(event)) {}

    inline EventType get_type() const {
        return std::visit([](const auto& e) { return get_type(e); }, *this);
    }

    template<class T>
    T& Get() { return std::get<T>(*this); }

    template<class T>
    const T& Get() const { return std::get<T>(*this); }

private:
    template<class T>
    static EventType get_type(const T&) { return T::Type(); }
};

}