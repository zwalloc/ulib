#pragma once

#include <ulib/encodings/charcase.h>

namespace ulib
{
    template <class StringT, class EncodingT = argument_encoding_or_die_t<StringT>>
    inline auto upper(const StringT &str)
    {
        return ToUpper(str);
    }

    template <class StringT, class EncodingT = argument_encoding_or_die_t<StringT>>
    inline auto lower(const StringT &str)
    {
        return ToLower(str);
    }
 
} // namespace ulib