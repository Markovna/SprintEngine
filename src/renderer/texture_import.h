#pragma once

#include "gfx.h"
#include "reflection/meta_runtime.h"

namespace sprint {

struct SERIALIZED TextureImport {
    SERIALIZABLE gfx::TextureFormat::Enum format = gfx::TextureFormat::RGBA8;
    SERIALIZABLE gfx::TextureWrap wrap_type = {};
    SERIALIZABLE gfx::TextureFilter filter = {};
    SERIALIZABLE gfx::TextureFlags::Type flags = gfx::TextureFlags::None;
};

}