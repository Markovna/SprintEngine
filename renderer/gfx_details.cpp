#include "gfx_details.h"
#include "renderer_api_gl.h"

namespace sprint {

namespace gfx {

namespace details {

std::unique_ptr<RendererAPI> CreateRendererApi(const Config& config) {
    return std::make_unique<GLRendererAPI>(config);
}

}

}

}