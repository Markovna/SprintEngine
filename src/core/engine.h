#pragma once

#include "common/time_span.h"
#include "renderer/gfx.h"

namespace sprint {

class Window;
class World;

class Engine {
public:
    static std::unique_ptr<Engine> Create();

    explicit Engine();
    ~Engine();

    void Update();

    [[nodiscard]] TimeSpan get_delta() const;
    [[nodiscard]] World* get_world();

private:
    Timer timer_;
    TimeSpan delta_time_;
    std::unique_ptr<World> world_;
};

}

