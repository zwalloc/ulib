#pragma once

#include <ulib/typetraits/allocator.h>
#include <ulib/typetraits/carray.h>
#include <ulib/typetraits/field.h>

#include <iterator>

#include <ulib/containers/tags.h>

namespace ulib
{
    // -----------

    ULIB_DEFINE_TYPE_FIELD_CHECK(container_value_type, value_type);
    ULIB_DEFINE_TYPE_FIELD_CHECK(container_element_type, element_type);
    ULIB_DEFINE_TYPE_FIELD_CHECK(container_iterator, iterator);
    ULIB_DEFINE_TYPE_FIELD_CHECK_T(container_iterator_tag, typename std::iterator_traits<typename T::iterator>::iterator_category);
    ULIB_DEFINE_TYPE_FIELD_CHECK_T(container_data_method, decltype(std::declval<T>().data()));
    ULIB_DEFINE_TYPE_FIELD_CHECK_T(container_size_method, decltype(std::declval<T>().size()));
    ULIB_DEFINE_TYPE_FIELD_CHECK_T(container_begin_method, decltype(std::declval<T>().begin()));
    ULIB_DEFINE_TYPE_FIELD_CHECK_T(container_end_method, decltype(std::declval<T>().end()));

    ULIB_DEFINE_TYPE_FIELD_CHECK_T(iterator_value_type, typename std::iterator_traits<T>::value_type);
    ULIB_DEFINE_TYPE_FIELD_CHECK_T(iterator_tag, typename std::iterator_traits<T>::iterator_category);

    ULIB_DEFINE_TYPE_FIELD_CHECK(ulib_container_type, ContainerTypeT);
    ULIB_DEFINE_TYPE_FIELD_CHECK(ulib_container_ownership, ContainerOwnershipT);
    ULIB_DEFINE_TYPE_FIELD_CHECK(ulib_container_allocator, AllocatorT);

    ULIB_DEFINE_TYPE_FIELD_CHECK(std_container_traits, traits_type);
    ULIB_DEFINE_TYPE_FIELD_CHECK(std_container_allocator, allocator_type);

    ULIB_DEFINE_TYPE_FIELD_CHECK_T(begin_result, decltype(std::begin(std::declval<T>())));
    ULIB_DEFINE_TYPE_FIELD_CHECK_T(end_result, decltype(std::end(std::declval<T>())));

    // -------------

    template <class T>
    inline constexpr bool has_ulib_container_string_type_v = is_ulib_container_type_v<T, string_type_tag>;

    template <class T>
    inline constexpr bool has_ulib_container_list_type_v = is_ulib_container_type_v<T, list_type_tag>;

    template <class T>
    inline constexpr bool has_ulib_container_category_v = is_ulib_container_type_v<T, map_type_tag>;

    // -------------

    template <class T>
    inline constexpr bool has_ulib_container_store_ownership_v = is_ulib_container_ownership_v<T, store_ownership_tag>;

    template <class T>
    inline constexpr bool has_ulib_container_view_ownership_v = is_ulib_container_ownership_v<T, view_ownership_tag>;

    // -------------

    template <class T>
    inline constexpr bool is_container_random_accessible_v = is_container_iterator_tag_v<T, std::random_access_iterator_tag>;

    template <class T>
    inline constexpr bool is_container_bidirectional_v = is_container_iterator_tag_v<T, std::bidirectional_iterator_tag>;

    template <class T>
    inline constexpr bool is_random_accessible_v = is_c_array_v<T> || is_container_random_accessible_v<T>;

    // template <class T>
    // inline constexpr bool has_container_allocator_v = has_std_container_allocator_v<T> || has_ulib_container_allocator_v<T>;

    // -----------

    template <class T>
    inline constexpr bool is_container_enumerable_v = has_container_begin_method_v<T> && has_container_end_method_v<T>;

    template <class T>
    inline constexpr bool is_enumerable_v = has_begin_result_v<T> && has_end_result_v<T>;

    // -----------

    template <class T, class = void>
    struct determined_value_type
    {
        using type = missing_type;
    };

    template <class T>
    struct determined_value_type<T, std::enable_if_t<is_c_array_v<T>>>
    {
        using type = c_array_value_type_t<T>;
    };

    template <class T>
    struct determined_value_type<T, std::enable_if_t<has_container_value_type_v<T>>>
    {
        using type = container_value_type_t<T>;
    };

    ULIB_DEFINE_TYPE_CHECKS(determined_value_type);
    ULIB_DEFINE_OR_DIE_TYPE_CHECK(determined_value_type);

    // -----------

    template <class T, class K>
    struct container_range_compatible
    {
        using value_type = container_value_type_t<T>;
        using k_value_type = container_value_type_t<K>;

        static constexpr bool is_tags_compatible =
            is_container_iterator_tag_v<T, std::random_access_iterator_tag> && is_container_iterator_tag_v<K, std::random_access_iterator_tag>;

        static constexpr bool is_type_compatible = std::is_same_v<std::remove_cv_t<value_type>, std::remove_cv_t<k_value_type>>;
        static constexpr bool value = is_tags_compatible && is_type_compatible;
    };

    template <class T, class K>
    inline constexpr bool is_container_range_compatible_v = container_range_compatible<T, K>::value;

    template <class T, class K>
    inline constexpr bool is_const_compatible_v = !(!std::is_const_v<T> && std::is_const_v<K>);

    template <class T, class K>
    using enable_if_range_compatible_t = std::enable_if_t<is_container_range_compatible_v<T, K>, bool>;

    template <class T, class K>
    using enable_if_not_same_t = std::enable_if_t<!std::is_same_v<T, K>, bool>;

    template <class T, class K>
    using enable_if_const_compatible_t = std::enable_if_t<is_const_compatible_v<T, K>, bool>;

    template <class SpanT, class K>
    struct span_from_range_constructible
    {
        using s_type = SpanT;
        using k_type = std::remove_reference_t<K>;

        using s_element_type = typename s_type::element_type;

        static constexpr bool is_cc =
            is_const_compatible_v<s_element_type, k_type> &&
            (!has_container_element_type_v<k_type> || is_const_compatible_v<s_element_type, container_element_type_t<k_type>>);

        static constexpr bool value = is_cc && !std::is_same_v<s_type, k_type> && is_container_range_compatible_v<s_type, k_type>;
    };

    template <class SpanT, class K>
    static constexpr bool is_span_from_range_constructible_v = span_from_range_constructible<SpanT, K>::value;

    template <class SpanT, class K>
    using enable_if_span_from_range_constructible_t = std::enable_if_t<is_span_from_range_constructible_v<SpanT, K>, bool>;

    template <class RangeT, class K>
    static constexpr bool is_container_from_range_constructible_v =
        !std::is_same_v<RangeT, std::remove_reference_t<K>> && is_container_range_compatible_v<RangeT, std::remove_reference_t<K>>;

    template <class RangeT, class K>
    using enable_if_container_from_range_constructible_t = std::enable_if_t<is_container_from_range_constructible_v<RangeT, K>, bool>;

    // construct view from range
    // construct container from range

    // template <class T, class K>
    // inline constexpr bool is_different_container_range_compatible_v = !std::is_same_v<T, K> && is_container_range_compatible_v<T, K>;

    // template <class T, class K>
    // using enable_if_range_compatible_t = std::enable_if_t<is_different_container_range_compatible_v<T, K>, bool>;

    // -----------

    template <class T, class = void>
    struct container_allocator
    {
        using type = missing_type;
    };

    template <class T>
    struct container_allocator<T, std::enable_if_t<has_ulib_container_allocator_v<T>>>
    {
        using type = ulib_container_allocator_t<T>;
    };

    template <class T>
    struct container_allocator<T, std::enable_if_t<has_std_container_allocator_v<T>>>
    {
        using type = std_container_allocator_t<T>;
    };

    ULIB_DEFINE_TYPE_CHECKS(container_allocator);
    // ULIB_DEFINE_OR_DIE_TYPE_CHECK(container_allocator);

    template <class T>
    using constainer_choose_ulib_allocator = choose_ulib_allocator<container_allocator_t<T>>;

    ULIB_DEFINE_TYPE_CHECKS(constainer_choose_ulib_allocator);
    ULIB_DEFINE_OR_DIE_TYPE_CHECK(constainer_choose_ulib_allocator);

} // namespace ulib