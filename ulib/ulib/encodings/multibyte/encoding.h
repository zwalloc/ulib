#pragma once

#include "../type.h"
#include "../nullencoding.h"

namespace ulib
{
    struct MultibyteEncoding
    {
        using CharT = char;

        constexpr static EncodingType kType = EncodingType::Raw;
        constexpr static EncodingCharType kCharType = EncodingCharType::SingleByte;
    };
}