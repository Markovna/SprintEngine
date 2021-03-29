#include "../../../../renderer/texture_import.h"

template<>
class ::meta::details::Reflection<sprint::TextureImport> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::TextureImport>("sprint::TextureImport");
        ::meta::details::Register<sprint::TextureImport*>("sprint::TextureImport");
    }

    static void InitFields() {
        ::meta::details::AddField<sprint::TextureImport, sprint::gfx::TextureFormat::Enum>("format", &get_format);
        ::meta::details::AddField<sprint::TextureImport, sprint::gfx::TextureWrap>("wrap_type", &get_wrap_type);
        ::meta::details::AddField<sprint::TextureImport, sprint::gfx::TextureFilter>("filter", &get_filter);
        ::meta::details::AddField<sprint::TextureImport, unsigned int>("flags", &get_flags);
    }

    static Reference get_format(Reference& instance) {
        return Reference(instance.Get<sprint::TextureImport>().format);
    }

    static Reference get_wrap_type(Reference& instance) {
        return Reference(instance.Get<sprint::TextureImport>().wrap_type);
    }

    static Reference get_filter(Reference& instance) {
        return Reference(instance.Get<sprint::TextureImport>().filter);
    }

    static Reference get_flags(Reference& instance) {
        return Reference(instance.Get<sprint::TextureImport>().flags);
    }
};
