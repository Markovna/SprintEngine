#pragma once

#include "meta_runtime.h"

class SERIALIZED MeshComponent {
private:
    SERIALIZABLE float float_value = 10.0f;
    SERIALIZABLE int int_value = 42;
    SERIALIZABLE bool bool_value = false;
};


