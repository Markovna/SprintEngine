#pragma once

#include <string>
#include "KeyCode.h"
#include "MouseCodes.h"

#define EVENT_STATIC_TYPE(__type) static EventType GetStaticType() { return EventType::__type; }

enum class EventType {
    NONE        = 0x000,
    MOUSE_UP    = 0x001,
    MOUSE_DOWN  = 0x002,
    MOUSE_MOVE  = 0x004,
    KEY_PRESS   = 0x008,
    KEY_RELEASE = 0x010
};

class Event {
public:
    virtual ~Event() = default;

    EventType GetType() const { return m_Type; }
    bool IsConsumed() const { return m_Consumed; }
    void Consume() { m_Consumed = true; }

protected:
    explicit Event(EventType);

private:
    const EventType m_Type;
    bool m_Consumed;
};

class MouseDownEvent : public Event {
public:
    explicit MouseDownEvent(MouseCode mouseCode) : Event(EventType::MOUSE_DOWN) {}
    EVENT_STATIC_TYPE(MOUSE_DOWN)
};

class MouseUpEvent : public Event {
public:
    explicit MouseUpEvent(MouseCode mouseCode) : Event(EventType::MOUSE_UP) {}
    EVENT_STATIC_TYPE(MOUSE_UP)
};

class MouseMoveEvent : public Event {
public:
    MouseMoveEvent(double x, double y) : Event(EventType::MOUSE_MOVE) {}
    EVENT_STATIC_TYPE(MOUSE_MOVE)
};

class KeyPressEvent : public Event {
public:
    explicit KeyPressEvent(KeyCode keyCode) : Event(EventType::KEY_PRESS) {}
    EVENT_STATIC_TYPE(KEY_PRESS)
};

class KeyReleaseEvent : public Event {
public:
    explicit KeyReleaseEvent(KeyCode keyCode) : Event(EventType::KEY_RELEASE) {}
    EVENT_STATIC_TYPE(KEY_RELEASE)
};