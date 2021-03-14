#pragma once

#include "common/time_span.h"
#include "renderer/gfx.h"
#include "scene.h"

namespace sprint {

class Window;

class Engine {
public:
    static std::unique_ptr<Engine> Create();

    explicit Engine();
    ~Engine();

    void Update();

    [[nodiscard]] TimeSpan get_delta() const;
    [[nodiscard]] Scene* get_scene();

private:
    Timer timer_;
    TimeSpan delta_time_;
    std::unique_ptr<Scene> scene_;
};

}
