#include "Delegate.h"
#include <iostream>
#include <sstream>
#include <array>

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

int Foo(int x) {
    return x + 1;
}

struct Counter {
    static size_t construct_count;
    static size_t destruct_count;
    static size_t assign_count;

    static void Reset() {
        construct_count = 0;
        destruct_count = 0;
        assign_count = 0;
    }

    int operator()(int x) const {
        return x + 1;
    }

    Counter() { construct_count++; }
    Counter(const Counter&) { construct_count++; }
    Counter(Counter&&) { construct_count++; }

    Counter& operator= (const Counter&) { assign_count++; return *this; }
    Counter& operator= (Counter&&) = delete;

    ~Counter() { destruct_count++; }
};

size_t Counter::construct_count = 0;
size_t Counter::destruct_count = 0;
size_t Counter::assign_count = 0;

struct NonCopyable {
    NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable(NonCopyable&&) = default;
    int operator()(int x, int y){ return x + y; }
};

void TestMemberFunc() {
    // member func
    {
        {
            Widget inst;
            delegate<void(int)> d(inst, &Widget::Foo);
            d(10);
            assert(inst.count == 10);
        }

        {
            const Widget inst;
            delegate<void(int)> d(inst, &Widget::Foo);
            d(42);
            assert(inst.count == 42);
        }

        // test copy
        {
            Widget inst;
            delegate<void(int)> d(inst, &Widget::Foo);
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
                delegate<void(int)> d_copy;
                d_copy = d;
                d_copy(1);
                assert(inst.count == 4);
            }
        }
    }
}

void TestFreeFunc() {

    {
        delegate<int(int)> d = AddOne;
        assert(d(1) == 2);
    }

    {
        delegate<int(int)> d(&AddOne);
        auto d_copy(d);
        assert(d_copy(1) == 2 && d(1) == 2);
    }

}

void TestLambda() {
    {
        int sum = 0;
        delegate<void(int)> d = [&sum](int x) {
            sum += x;
        };
        d(1);
        assert(sum == 1);
    }

    {
        std::array<char, 1024> arr{};
        int sum = 0;
        delegate<void(int)> d([&sum, arr](int x) {
            sum += x;
        });
        d(10);
        assert(sum == 10);

        {
            delegate<void(int)> d_copy = d;
            d_copy(10);
            assert(sum == 20);

            d(10);
            assert(sum == 30);
        }
    }
}

void TestFunctor() {
    {
        {
            Counter::Reset();
            Counter counter;
            auto d = delegate<int(int)>(counter);
            assert(d(1) == 2);
        }
        assert(Counter::construct_count == 2);
        assert(Counter::destruct_count == 2);
        assert(Counter::assign_count == 0);
    }

    // test copy constructor
    {
        {
            Counter::Reset();
            Counter counter;
            auto d = delegate<int(int)>(counter);
            auto copy_d = d;

            assert(copy_d(11) == 12);
        }
        assert(Counter::construct_count == 3);
        assert(Counter::destruct_count == 3);
        assert(Counter::assign_count == 0);
    }

    // test assign operator
    {
        {
            Counter::Reset();
            Counter counter;
            auto d = delegate<int(int)>(counter);
            delegate<int(int)> copy_d;
            copy_d = d;

            assert(copy_d(41) == 42);
        }
        assert(Counter::construct_count == 3);
        assert(Counter::destruct_count == 3);
        assert(Counter::assign_count == 0);
    }
}


void TestCompare() {
    /**/
    {
        Widget inst;
        Widget inst2;
        delegate<void(int)> d1(inst, &Widget::Foo);
        delegate<void(int)> d2(inst, &Widget::Foo);
        delegate<void(int)> d3(&inst, &Widget::Bar);
        delegate<void(int)> d4(&inst2, &Widget::Bar);
        assert(d1 == d2);
        assert(d1 != d3);
        assert(d1 != d4);
        assert(d3 != d4);
    }

    {
        delegate<int(int)> d1 = AddOne;
        delegate<int(int)> d2 = AddOne;
        delegate<int(int)> d3 = Foo;
        assert(d1 == d2);
        assert(d1 != d3);
    }

    {
        std::array<char, 1024> arr;
        int sum = 0;
        auto lambda = [](int x) {
            return x;
        };

        delegate<int(int)> d1 = lambda;
        delegate<int(int)> d2 = d1;
        assert(d1 != d2);
    }
    /**/
}


void TestCommon() {
    // test return value
    {
        delegate<int(int,int)> d = [](int x, int y) { return x + y; };
        assert(d(10, 42) == 52);
        assert(d != nullptr);
        assert(d);
    }

    {
        delegate<int(int, int)> d = NonCopyable{};
        assert(d(10, 42) == 52);
    }

    {
        struct BigNonCopyable : NonCopyable {
            std::array<char, 1024> arr;
        };

        delegate<int(int, int)> d = BigNonCopyable{};
        assert(d(10, 42) == 52);
    }

    {
        delegate<int(int, int)> d = NonCopyable{};
        delegate<int(int, int)> d_copy = std::move(d);
        assert(d_copy(10, 42) == 52);
        assert(d == nullptr);
    }

    {
        delegate<int(int)> d = nullptr;
        d = [](int x){ return x + 1; };
        assert(d(1) == 2);
    }
    {
        Counter::Reset();
        Counter counter;
        delegate<int(int)> d(counter);
        d = nullptr;
        assert(Counter::construct_count == 2);
        assert(Counter::destruct_count == 1);
        assert(Counter::assign_count == 0);
    }
    {
        Counter::Reset();
        Counter counter;
        delegate<int(int)> d = nullptr;
        delegate<int(int)> d2 = counter;
        d2 = d;
        assert(!d && !d2);
        assert(Counter::construct_count == 2);
        assert(Counter::destruct_count == 1);
        assert(Counter::assign_count == 0);
    }
    {
        delegate<int(int)> d = AddOne;
        assert(d(41) == 42);
    }

    // self copy assign
    {
        delegate<int(int)> d = AddOne;
        d = d;
        assert(d(1) == 2);
    }
    {
        std::array<char, 1024> arr{};
        delegate<int(int)> d = [arr](int x){ return x + 1; };
        d = d;
        assert(d(1) == 2);
    }
}

void TestDelegates() {
    TestMemberFunc();
    TestFreeFunc();
    TestLambda();
    TestFunctor();
    TestCommon();
    TestCompare();
    std::cout << "TestDelegates: OK\n";
}