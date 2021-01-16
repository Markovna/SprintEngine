#pragma once

#include <time_span.h>
#include "app_time.h"
#include "Window.h"

namespace sprint {

class Engine {
public:
    explicit Engine(const Window&);
    ~Engine();
    void Update();
};

}

