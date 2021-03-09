#pragma once

#include <string>

namespace sprint::resources {

namespace details {

class ResourceRegistry {
public:
    template<class T>
    T Load(const std::string& path);

};

ResourceRegistry& Registry() {
    static ResourceRegistry registry;
    return registry;
}

}

template<class T>
T Load(const std::string& path) {
    return details::Registry().Load<T>();
}

}