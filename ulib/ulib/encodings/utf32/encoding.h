#pragma once

#include <ulib/types.h>
#include "../uchars.h"
#include "../type.h"
#include "../nullencoding.h"

namespace ulib
{
    struct Utf32
    {
        using CharT = char32;

        constexpr static EncodingType kType = EncodingType::Concrete;
        constexpr static EncodingCharType kCharType = EncodingCharType::SingleByte;

#ifdef __cpp_char8_t
        using CharStd = int;
#endif
        inline static CharT *Encode(uint codepoint, CharT *out)
        {
            *out = codepoint;
            return out + 1;
        }

        inline static const CharT *Decode(const CharT *begin, const CharT *end, uint &out)
        {
            out = *begin;
            return begin + 1;
        }
    };
}
