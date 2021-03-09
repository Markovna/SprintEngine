#pragma once

#include "type.h"
#include <iostream>

namespace meta {

class Object {
private:
    constexpr static size_t max_size  = sizeof(void*);
    constexpr static size_t max_align = alignof(void*);

    using storage = std::aligned_storage_t<max_size>;

    struct VTable {
        using destroy_ptr_t = void (*)(void* const);
        using get_ptr_t = void* (*)(const void* const);

        Type type;
        destroy_ptr_t destroy_ptr;
        get_ptr_t get_ptr;

        VTable(const VTable&) = delete;
        VTable(VTable&&) = delete;
        VTable& operator= (const VTable&) = delete;
        VTable& operator= (VTable&&) = delete;
        ~VTable() = default;

        template<class T>
        static constexpr bool store_locally =
            std::is_trivially_copyable<T>::value &&
                sizeof(T) <= max_size && alignof(T) <= max_align;

        template<class T>
        using local_storage = std::integral_constant<bool, store_locally<T>>;

        template<class T>
        static void Create(T&& object, void* store_ptr) {
            using F = std::decay_t<T>;
            CreateImpl(std::forward<T>(object), store_ptr, local_storage<F>());
        }

        template<class T>
        static void Destroy(void* const object_ptr) {
            DestroyImpl<T>(object_ptr, local_storage<T>());
        }

        template<class T>
        static void* Get(const void* const object_ptr) {
            return static_cast<void*>(&Access<T>(const_cast<void* const>(object_ptr)));
        }

        template<class T>
        static T& Access(void* const object_ptr) {
            return AccessImpl<T>(object_ptr, local_storage<T>());
        }

        template<class T>
        static T& AccessImpl(void* const object_ptr, std::true_type /* stored_locally */) {
            return *static_cast<T*>(object_ptr);
        }

        template<class T>
        static T& AccessImpl(void* const object_ptr, std::false_type /* stored_locally */) {
            return **static_cast<T**>(object_ptr);
        }

        template<class T>
        static void CreateImpl(T&& object, void* store_ptr, std::true_type /* stored_locally */) {
            using F = std::decay_t<T>;
            new (store_ptr) F (std::forward<T>(object));
        }

        template<class T>
        static void CreateImpl(T&& object, void* store_ptr, std::false_type /* stored_locally */) {
            using F = std::decay_t<T>;
            new (store_ptr) F* (new F (std::forward<T>(object)));
        }

        template<class T>
        static void DestroyImpl(void* const object_ptr, std::true_type /* stored_locally */) {
            Access<T>(object_ptr).~T();
        }

        template<class T>
        static void DestroyImpl(void* const object_ptr, std::false_type /* stored_locally */) {
            delete &Access<T>(object_ptr);
        }

        template<class T>
        static const VTable* GetTablePtr() {
            static const VTable vt{
                meta::GetType<T>(), Destroy<T>, Get<T>
            };
            return &vt;
        }
    };

public:
    template<
        class T,
        class = std::enable_if_t<!std::is_same<Object, std::decay_t<T>>::value>
        >
    Object(T&& obj) {
        vtable_ptr_ = VTable::GetTablePtr<std::decay_t<T>>();
        VTable::Create(std::forward<T>(obj), &store_);
    }

    Object() = default;

    Object(Object&& other) noexcept {
        Swap(other);
    }

    Object& operator=(Object&& other) noexcept {
        Object(std::move(other)).Swap(*this);
        return *this;
    }

    Object(const Object& o) = delete;
    Object& operator=(const Object& o) = delete;

    ~Object() {
        if (vtable_ptr_)
            vtable_ptr_->destroy_ptr(&store_);
    }

    template<class T>
    T& Get() const {
//        using F = typename std::remove_const<T>::type;
        return *static_cast<std::decay_t<T>*>(vtable_ptr_->get_ptr(&store_));
    }

    template<class T>
    [[nodiscard]] bool Is() const {
        return vtable_ptr_ ? vtable_ptr_->type.Is<T>() : false;
    }

    [[nodiscard]] Type GetType() const {
        return vtable_ptr_ ? vtable_ptr_->type : Type::Invalid();
    }

private:
    void Swap(Object& other) noexcept {
        std::swap(store_,      other.store_);
        std::swap(vtable_ptr_, other.vtable_ptr_);
    }

private:
    const VTable* vtable_ptr_ = nullptr;
    storage store_{};
};

}