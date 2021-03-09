#include "../../../../../core/components/mesh_renderer.h"

template<>
class ::meta::details::Reflection<sprint::MeshRenderer> {
public:
    static void Allocate() {
        ::meta::details::Register<sprint::MeshRenderer>("sprint::MeshRenderer");
        ::meta::details::Register<sprint::MeshRenderer*>("sprint::MeshRenderer");
    }

    static void InitFields() {
    }
};
