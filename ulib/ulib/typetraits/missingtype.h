#pragma once

namespace ulib
{
    struct missing_type
    {
        missing_type() = delete;
        ~missing_type() = delete;
    };

    template <class TagT>
    struct undefined_type
    {
        inline undefined_type(...) {}
    };

    template <class T, class TagT>
    struct undefined_if_missing
    {
        using type = T;
    };

    template <class TagT>
    struct undefined_if_missing<missing_type, TagT>
    {
        using type = undefined_type<TagT>;
    };

    template <class T, class TagT>
    using undefined_if_missing_t = typename undefined_if_missing<T, TagT>::type;

    template <class T, bool BExprValue>
    struct type_if
    {
        using type = T;     
    };

    template <class T>
    struct type_if<T, false>
    {
        using type = missing_type;
    };

    template <class T, bool BExprValue>
    using type_if_t = typename type_if<T, BExprValue>::type;
} // namespace ulib