#pragma once

#include <iomanip>
#include <string>
#include <vector>
#include <iostream>
#include <cstdio>
#include <thread>
#include <cassert>

class Test {
    int field;
    std::string str_field;

    void some_method(uint32_t val, std::vector<long>);
    static void some_static_method(const std::istream&);
};