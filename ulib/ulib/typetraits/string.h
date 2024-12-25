#pragma once

#include <iterator>
#include <string>
#include <type_traits>

#include <ulib/containers/tags.h>

#include <ulib/typetraits/container.h>
#include <ulib/typetraits/literalencoding.h>

namespace ulib
{
    template <class T, class = void, class = void, class = void>
    struct is_std_string : std::false_type
    {
    };

    template <class T>
    struct is_std_string<T, std::void_t<typename T::value_type>, std::void_t<typename T::traits_type>, std::void_t<typename T::allocator_type>>
        : std::bool_constant<std::is_same_v<T, std::basic_string<typename T::value_type, typename T::traits_type, typename T::allocator_type>>>
    {
    };

    template <class T>
    inline constexpr bool is_std_string_v = is_std_string<T>::value;

    // -----------

    template <class T, class = void>
    struct is_std_string_view : std::false_type
    {
    };

    template <class T>
    struct is_std_string_view<T, std::void_t<typename T::value_type>>
        : std::bool_constant<std::is_same_v<T, std::basic_string_view<typename T::value_type>>>
    {
    };

    template <class T>
    inline constexpr bool is_std_string_view_v = is_std_string_view<T>::value;

    // -----------

    template <class T>
    inline constexpr bool is_std_string_kind_v = is_std_string_view_v<T> || is_std_string_v<T>;

    // -----------

    template <class T>
    inline constexpr bool is_ulib_string_v = is_ulib_container_type_v<T, string_type_tag> && is_ulib_container_ownership_v<T, store_ownership_tag>;

    template <class T>
    inline constexpr bool is_ulib_string_view_v =  is_ulib_container_type_v<T, string_type_tag> && is_ulib_container_ownership_v<T, view_ownership_tag>;

    template <class T>
    inline constexpr bool is_ulib_string_kind_v = is_ulib_container_type_v<T, string_type_tag>;

    // -----------

    template <class T>
    inline constexpr bool is_string_v = is_ulib_string_v<T> || is_std_string_v<T>;

    template <class T>
    inline constexpr bool is_string_view_v = is_ulib_string_view_v<T> || is_std_string_view_v<T>;

    template <class T>
    inline constexpr bool is_string_kind_v = is_ulib_string_kind_v<T> || is_std_string_kind_v<T>;

} // namespace ulib

namespace ulib
{
    template <class T, class = void>
    struct string_encoding
    {
        using type = ulib::missing_type;
    };

    template <class T>
    struct string_encoding<T, std::void_t<typename T::EncodingT>>
    {
        using type = typename T::EncodingT;
    };

    template <class T>
    struct string_encoding<T, std::enable_if_t<is_std_string_kind_v<T>>>
    {
        using type = literal_encoding_t<typename T::value_type>;
    };

    ULIB_DEFINE_TYPE_CHECKS(string_encoding);

} // namespace ulib

namespace ulib
{
    // -----------

    template <class T, class = void>
    struct argument_encoding
    {
        using type = missing_type;
    };

    template <class T>
    struct argument_encoding<T, std::enable_if_t<std::is_pointer_v<T>>>
    {
        using type = literal_encoding_t<std::remove_const_t<std::remove_pointer_t<T>>>;
    };

    template <class T>
    struct argument_encoding<T, std::enable_if_t<std::is_array_v<T>>>
    {
        using type = literal_encoding_t<std::remove_const_t<std::remove_all_extents_t<T>>>;
    };

    template <class T>
    struct argument_encoding<T, std::enable_if_t<is_string_kind_v<T>>>
    {
        using type = string_encoding_t<T>;
    };

    ULIB_DEFINE_TYPE_CHECKS(argument_encoding);
    ULIB_DEFINE_OR_DIE_TYPE_CHECK(argument_encoding);




} // namespace ulib