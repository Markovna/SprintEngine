
#include "gfx_details.h"
#include "renderer_api_gl.h"

namespace sprint::gfx::details {

std::unique_ptr<RendererAPI> CreateRendererApi(const Config &config) {
    return std::make_unique<GLRendererAPI>(config);
}

DrawUnit &Frame::GetDraw() {
    assert(draws_count_ < static_config::kMaxDrawCallsCount);
    return draws_[draws_count_];
}

void Frame::Reset() {
    draws_count_ = 0;
    Clear(draws_[draws_count_]);
    command_buffer_.clear();
}

void Frame::Next() {
    draws_count_++;
    Clear(draws_[draws_count_]);
}

};