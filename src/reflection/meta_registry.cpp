#include "meta_registry.h"


namespace meta::details {

const TypeInfo& GetTypeInfo(meta::TypeId id) {
    const MetaRegistry &registry = Registry();
    return registry.Get(id);
}

const TypeInfo& GetTypeInfo(std::string name) {
    const MetaRegistry &registry = Registry();
    return registry.Get(name);
}

}