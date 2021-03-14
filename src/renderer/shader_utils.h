#pragma once

#include "gfx.h"

namespace sprint {

namespace gfx {

void PreProcess(
    const std::string &source,
    std::string& vertex_shader,
    std::string& fragment_shader,
    Attribute::BindingPack& bindings);

}

}

