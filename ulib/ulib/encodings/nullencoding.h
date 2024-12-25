#pragma once

#include "type.h"

namespace ulib
{
    template <class CharType>
    struct NullEncoding
    {
        using CharT = CharType;
        using ParentEncodingT = NullEncoding<CharType>;
#ifdef __cpp_char8_t
        using CharStd = int;
#endif
        constexpr static EncodingType kType = EncodingType::Raw;
    };
}