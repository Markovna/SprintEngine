#include "function_ref.h"
#include <array>
#include <iostream>

/*
struct Widget {
    Widget() {}

    mutable int count = 0;
    void Foo(int x) { count += x; }
    void Foo(int x) const { count += x; }

    void Bar(int x) { }
};

static int AddOne(int x) {
    return x + 1;
}

static int Foo(int x) { return x; }

void TestMemberFuncRef() {
    // member func
    {
        {
            Widget inst;
            function_ref<void(int)> d(&inst, &Widget::Foo);
            d(10);
            assert(inst.count == 10);
        }

        {
            const Widget inst;
            function_ref<void(int)> d(&inst, &Widget::Foo);
            d(42);
            assert(inst.count == 42);
        }

        // test copy
        {
            Widget inst;
            function_ref<void(int)> d(&inst, &Widget::Foo);
            d(1);

            {
                auto d_copy(d);
                d_copy(1);
                assert(inst.count == 2);
            }

            {
                auto tmp(d);
                auto d_copy(std::move(tmp));
                d_copy(1);
                assert(inst.count == 3);
            }

            {
                function_ref<void(int)> d_copy;
                d_copy = d;
                d_copy(1);
                assert(inst.count == 4);
            }
        }
    }
}

void TestFreeFuncRef() {

    {
        function_ref<int(int)> d = AddOne;
        assert(d(1) == 2);
    }

    {
        function_ref<int(int)> d(&AddOne);
        auto d_copy(d);
        assert(d_copy(1) == 2 && d(1) == 2);
    }
}

void TestLambdaRef() {
    {
        auto lambda = [](int x) { return x + 1; };
        function_ref<int(int)> d = lambda;
        assert(d(1) == 2);
    }
    {
        int sum = 0;
        auto lambda = [&sum](int x) { sum += x; };
        function_ref<void(int)> d = lambda;
        d(1);
        assert(sum == 1);
    }
    {
        std::array<char, 1024> arr{};
        int sum = 0;
        auto lambda = [&sum, arr](int x) { sum += x; };
        function_ref<void(int)> d(lambda);
        d(10);
        assert(sum == 10);

        {
            function_ref<void(int)> d_copy = d;
            d_copy(10);
            assert(sum == 20);

            d(10);
            assert(sum == 30);
        }
    }
}

void TestCompare() {
    {
        Widget inst;
        Widget inst2;
        function_ref<void(int)> d1(&inst, &Widget::Foo);
        function_ref<void(int)> d2(&inst, &Widget::Foo);
        function_ref<void(int)> d3(&inst, &Widget::Bar);
        function_ref<void(int)> d4(&inst2, &Widget::Bar);
        assert(d1 == d2);
        assert(d1 != d3);
        assert(d1 != d4);
        assert(d3 != d4);
    }

    {
        function_ref<int(int)> d1 = AddOne;
        function_ref<int(int)> d2 = AddOne;
        function_ref<int(int)> d3 = d1;
        function_ref<int(int)> d4 = Foo;
        assert(d1 == d2);
        assert(d1 == d3);
        assert(d2 == d3);
        assert(d1 != d4);
    }

    {
        int sum = 0;
        auto lambda = [&sum](int x) { sum += x; };
        auto lambda2 = [&sum](int x) { sum -= x; };
        function_ref<void(int)> d1 = lambda;
        function_ref<void(int)> d2 = lambda;
        function_ref<void(int)> d3 = lambda2;
        function_ref<void(int)> d4 = nullptr;
        assert(d1 == d2);
        assert(d1 != d3);
        assert(d2 != d3);
        assert(d3 != d4);
        d4 = d3;
        assert(d4 == d3);
    }
}
*/

void TestFunctionRef() {
//    TestMemberFuncRef();
//    TestFreeFuncRef();
//    TestLambdaRef();
//    TestCompare();
}