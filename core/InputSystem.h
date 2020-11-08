#pragma once

#include "WindowEvent.h"

namespace sprint {

class InputSystem {
public:
    void PushEvent(WindowEvent &event) {
        switch (event.get_type()) {
//            case WindowEvent::Type::MOUSE_DOWN:  OnMouseDown(event.Mouse);     break;
//            case WindowEvent::Type::MOUSE_UP:    OnMouseUp(event.Mouse);       break;
//            case WindowEvent::Type::MOUSE_MOVE:  OnMouseMove(event.MouseMove); break;
//            case WindowEvent::Type::KEY_PRESS:   OnKeyPress(event.Key);        break;
//            case WindowEvent::Type::KEY_RELEASE: OnKeyRelease(event.Key);      break;
            default: break;
        }
    }
};

}


