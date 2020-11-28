#pragma once

#include <cassert>
#include <memory>
#include <new>
#include <type_traits>
#include <utility>
#include <functional>

/**
 *
 *
 * event.connect(make_function_ref<void(int), Widget, &Widget::Foo>(this));
 *
**/

template<class T>
class function_ref;

template<class R, class ...A>
class function_ref<R(A...)> {
private:
    using signature_type = R(void*, A...);

    void* _ptr;
    R (*_erased_fn)(void*, A...);

public:
    template <
            typename T,
            typename = std::enable_if<!std::is_same<std::decay_t<T>, function_ref>::type>
            >
    function_ref(T&& x) noexcept : _ptr{(void*)std::addressof(x)}
    {
        _erased_fn = [](void* ptr, A... xs) -> R {
            return (*reinterpret_cast<std::add_pointer_t<T>>(ptr))(
                    std::forward<A>(xs)...);
        };
    }

    function_ref(void* ptr, R (*erased_fn)(void*, A...)) : _ptr(ptr), _erased_fn(erased_fn)
    {}

    R operator()(A... xs) const noexcept
    (noexcept(_erased_fn(_ptr, std::forward<A>(xs)...))) {
        return _erased_fn(_ptr, std::forward<A>(xs)...);
    }

    template <class C, R (C::*member_ptr)(A...)>
    static function_ref from(C* object) {
        return function_ref((void*)object, &member_func_stub<C, member_ptr>);
    }

    template <class C, R (C::*member_ptr)(A...)>
    static R member_func_stub(void* object, A... args) {
        return ((static_cast<C*>(object))->*member_ptr)(std::forward<A>(args)...);
    }
};

struct Widget2 {
public:
    int Foo(int x) { return x; }
//    int Foo(int x, int y) { return x + y; }

    void Test() {
        
        auto func = function_ref<int(int)>::from<Widget2, &Widget2::Foo>(this);
        func(1);

    }
};

inline void Test() {
    Widget2 inst;
    inst.Test();
}

void TestFunctionRef();
