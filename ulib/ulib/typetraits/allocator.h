#pragma once

#include <memory>
#include <ulib/allocators/defaultallocator.h>
#include <ulib/typetraits/field.h>

namespace ulib
{
    ULIB_DEFINE_TYPE_FIELD_CHECK_T(std_allocator_allocate_method, decltype(std::declval<T>().allocate(10)))
    ULIB_DEFINE_TYPE_FIELD_CHECK_T(ulib_allocator_alloc_method, decltype(std::declval<T>().Alloc(10)))

    // ----------------
    template <class T, class = void>
    struct is_default_std_allocator : std::false_type
    {
    };

    template <class T>
    struct is_default_std_allocator<T, std::void_t<typename T::value_type>>
        : std::bool_constant<std::is_same_v<T, std::allocator<typename T::value_type>>>
    {
    };

    template <class T>
    inline constexpr bool is_default_std_allocator_v = is_default_std_allocator<T>::value;

    // ----------------

    template <class T>
    inline constexpr bool is_default_ulib_allocator_v = std::is_same_v<T, ulib::DefaultAllocator>;

    // ----------------

    template <class AllocatorT, class StdAllocatorT>
    inline constexpr bool is_same_ulib_and_std_allocators = is_default_ulib_allocator_v<AllocatorT> && is_default_std_allocator_v<StdAllocatorT>;

    // ----------------

    template <class T, class = void>
    struct choose_ulib_allocator
    {
        using type = ulib::DefaultAllocator;
    };

    template <class T>
    struct choose_ulib_allocator<T, std::enable_if_t<has_ulib_allocator_alloc_method_v<T>>>
    {
        using type = T;
    };

    template <class T>
    struct choose_ulib_allocator<T, std::enable_if_t<has_std_allocator_allocate_method_v<T>>>
    {
        // here need to reproduce some shit
        using type = ulib::DefaultAllocator;
    };

    ULIB_DEFINE_TYPE_CHECKS(choose_ulib_allocator);
    ULIB_DEFINE_OR_DIE_TYPE_CHECK(choose_ulib_allocator);

    // ----------------
} // namespace ulib