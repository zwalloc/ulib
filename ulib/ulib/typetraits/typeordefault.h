#pragma once

#include <ulib/typetraits/missingtype.h>

namespace ulib
{
    template <class T, class DefaultT>
    struct type_or_default
    {
        using type = T;
    };

    template <class DefaultT>
    struct type_or_default<missing_type, DefaultT>
    {
        using type = DefaultT;
    };

    template <class T, class DefaultT>
    using type_or_default_t = typename type_or_default<T, DefaultT>::type;
}