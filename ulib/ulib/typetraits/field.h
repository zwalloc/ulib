#pragma once

#include <type_traits>
#include <ulib/typetraits/missingtype.h>

#define ULIB_DEFINE_TYPE_CHECKS(check_name)                                                                                                          \
    template <class K>                                                                                                                               \
    using check_name##_t = typename check_name<K>::type;                                                                                             \
    template <class T>                                                                                                                               \
    inline constexpr bool has_##check_name##_v = !std::is_same_v<check_name##_t<T>, ulib::missing_type>;                                             \
    template <class T, class T1>                                                                                                                     \
    inline constexpr bool is_##check_name##_v = std::is_same_v<check_name##_t<T>, T1>;

#define ULIB_DEFINE_OR_DIE_TYPE_CHECK(check_name)                                                                                                    \
    template <class T, class = void>                                                                                                                 \
    struct check_name##_or_die                                                                                                                       \
    {                                                                                                                                                \
    };                                                                                                                                               \
    template <class T>                                                                                                                               \
    struct check_name##_or_die<T, std::enable_if_t<has_##check_name##_v<T>>>                                                                         \
    {                                                                                                                                                \
        using type = check_name##_t<T>;                                                                                                              \
    };                                                                                                                                               \
    template <class T>                                                                                                                               \
    using check_name##_or_die_t = typename check_name##_or_die<T>::type;

#define ULIB_DEFINE_TYPE_FIELD_CHECK_T(check_name, type_statement)                                                                                   \
    template <class T, class = void>                                                                                                                 \
    struct check_name                                                                                                                                \
    {                                                                                                                                                \
        using type = missing_type;                                                                                                                   \
    };                                                                                                                                               \
    template <class T>                                                                                                                               \
    struct check_name<T, std::void_t<type_statement>>                                                                                                \
    {                                                                                                                                                \
        using type = type_statement;                                                                                                                 \
    };                                                                                                                                               \
    ULIB_DEFINE_TYPE_CHECKS(check_name)

#define ULIB_DEFINE_TYPE_FIELD_CHECK(check_name, type_statement) ULIB_DEFINE_TYPE_FIELD_CHECK_T(check_name, typename T::type_statement)
