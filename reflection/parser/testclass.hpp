#pragma once

#include <iomanip>
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include <thread>
#include <cassert>
#include "../runtime.h"

namespace sprint {

class SERIALIZED TestClass {
public:
    int public_int_field;

private:

    class SERIALIZED NestedClass {
        FIELD()
        int nested_data;
    };

    FIELD()
    int int_field_;

    FIELD()
    float float_field_;

};

//class TestClass {
//    SERIALIZABLE()
//
//    class NestedClass;
//
//    META()
//    int field;
//
//    META()
//    std::string str_field;
//
//    META()
//    void some_method(uint32_t val, std::vector<long>);
//
//    META()
//    static void some_static_method(const std::istream &);
//};
//
//class TestClass::NestedClass {
//        SERIALIZABLE()
//
//        META()
//        int nested_class_field;
//};


}