#pragma once

#include <type_traits>

#include <ulib/typetraits/field.h>

namespace ulib
{
    template <class T>
    struct is_c_array : std::false_type
    {
    };

    template <class T, int N>
    struct is_c_array<T[N]> : std::true_type
    {
    };

    template <class T>
    inline constexpr bool is_c_array_v = is_c_array<T>::value;

    template <class T>
    struct c_array_value_type
    {
        using type = missing_type;
    };

    template <class T, int N>
    struct c_array_value_type<T[N]>
    {
        using type = std::remove_all_extents_t<T>;
    };

    ULIB_DEFINE_TYPE_CHECKS(c_array_value_type);
    
} // namespace ulib