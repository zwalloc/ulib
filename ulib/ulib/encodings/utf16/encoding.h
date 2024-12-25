#pragma once

#include <ulib/types.h>
#include "../exceptions.h"
#include "../uchars.h"
#include "../type.h"
#include "../nullencoding.h"

#include <ulib/typetraits/missingtype.h>

namespace ulib
{
    struct Utf16
    {
        using CharT = char16;

        constexpr static EncodingType kType = EncodingType::Concrete;
        constexpr static EncodingCharType kCharType = EncodingCharType::MultiByte;

        inline static CharT *Encode(uint codepoint, CharT *out)
        {
            if (codepoint <= 0xFFFF)
            {
                // surrogate check
                if (codepoint >= 0xD800 && codepoint <= 0xDFFF)
                {
                    // error

                    throw InvalidCodePointException{};
                }

                // 2 bytes utf16 character
                out[0] = ushort(codepoint);
                return &out[1];
            }
            else
            {
                // out of range check
                if (codepoint > 0x10FFFF)
                {
                    // error
                    throw UnsupportedEncodingException{};
                }

                // 4 bytes utf16 character
                uint bits = codepoint - 0x10000;
                out[0] = 0xD800 + (bits >> 10);
                out[1] = 0xDC00 + (bits & 0x3ff);

                return &out[2];
            }
        }

        inline static const CharT *Decode(const CharT *begin, const CharT *end, uint &out)
        {
            auto it = (ushort *)begin;

            if ((it[0] & 0xFC00) == 0xD800)
            {
                // first 0xD800 - 0xDBFF
                // 4 bytes

                if (it + 1 == (ushort *)end)
                {
                    throw CharacterOutOfRangeException{};
                }

                ushort first = (it[0] - 0xD800) << 10;
                ushort second = it[1] - 0xDC00;
                out = uint(first) + uint(second) + 0x10000;

                return (CharT *)&it[2];
            }
            else
            {
                out = it[0];
                return (CharT *)&it[1];
            }
        }

        inline static const CharT *NextChar(const CharT *begin, const CharT *end)
        {
            auto it = (ushort *)begin;

            if ((it[0] & 0xFC00) == 0xD800)
            {
                // first 0xD800 - 0xDBFF
                // 4 bytes

                if (it + 1 == (ushort *)end)
                {
                    throw CharacterOutOfRangeException{};
                }

                return (CharT *)&it[2];
            }
            else
            {
                return (CharT *)&it[1];
            }
        }
    };
}