#pragma once

#include <string>

enum class KeyCode {    // TODO: add all key codes
    A
};

class Event {
public:
    enum class Type {
        NONE        = 0x000,
        MOUSE_UP    = 0x001,
        MOUSE_DOWN  = 0x002,
        MOUSE_MOVE  = 0x004,
        KEY_PRESS   = 0x008,
        KEY_RELEASE = 0x010
    };

    virtual ~Event() = default;

    Type GetType() const { return m_Type; }
    void Consume() { m_Consumed = true; }

protected:
    Event(Type);

private:
    const Type m_Type;
    bool m_Consumed;
};

class MouseDownEvent : public Event {
public:
    MouseDownEvent() : Event(Type::MOUSE_DOWN) {}
};

class MouseUpEvent : public Event {
public:
    MouseUpEvent() : Event(Type::MOUSE_UP) {}
};

class MouseMoveEvent : public Event {
public:
    MouseMoveEvent(double x, double y) : Event(Type::MOUSE_MOVE) {}
};

class KeyPressEvent : public Event {
public:
    KeyPressEvent(KeyCode keyCode) : Event(Type::KEY_PRESS) {}
};

class KeyReleaseEvent : public Event {
public:
    KeyReleaseEvent(KeyCode keyCode) : Event(Type::KEY_RELEASE) {}
};