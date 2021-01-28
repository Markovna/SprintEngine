#pragma once

#include <time_span.h>
#include "Window.h"
#include "gfx.h"
#include "scene.h"

namespace sprint {

class Engine {
public:
    static std::unique_ptr<Engine> Create(const Window&);

    explicit Engine(const Window&);
    ~Engine();

    void Update();

    void SetOutput(gfx::framebuf_handle);
    [[nodiscard]] TimeSpan get_delta() const;

private:
    Timer timer_;
    TimeSpan delta_time_;
    std::unique_ptr<Scene> scene_;
};

}

