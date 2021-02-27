#include "meta_module.h"
#include "meta_module/common/color.gen.h"
#include "meta_module/common/rect.gen.h"
#include "meta_module/core/components/mesh_component.gen.h"
#include "meta_module/core/components/transform_component.gen.h"
#include "meta_module/math/quat.gen.h"
#include "meta_module/math/transform.gen.h"
#include "meta_module/math/vector.gen.h"

namespace meta {

void InitTypes() {

    details::RegisterPrimitiveTypes();

    details::Reflection<sprint::Color>::Allocate();
    details::Reflection<sprint::Rect>::Allocate();
    details::Reflection<MeshComponent>::Allocate();
    details::Reflection<sprint::TransformComponent>::Allocate();
    details::Reflection<sprint::Quat>::Allocate();
    details::Reflection<sprint::Transform>::Allocate();
    details::Reflection<sprint::vector<2>>::Allocate();
    details::Reflection<sprint::vector<3>>::Allocate();

    details::Reflection<sprint::Color>::InitFields();
    details::Reflection<sprint::Rect>::InitFields();
    details::Reflection<MeshComponent>::InitFields();
    details::Reflection<sprint::TransformComponent>::InitFields();
    details::Reflection<sprint::Quat>::InitFields();
    details::Reflection<sprint::Transform>::InitFields();
    details::Reflection<sprint::vector<2>>::InitFields();
    details::Reflection<sprint::vector<3>>::InitFields();
}

}