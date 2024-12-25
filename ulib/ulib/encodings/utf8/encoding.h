#pragma once

#include <ulib/types.h>
#include "../exceptions.h"
#include "../uchars.h"
#include "../type.h"
#include "../multibyte/encoding.h"

namespace ulib
{
    struct Utf8
    {
        using CharT = char8;
        using ParentEncodingT = MultibyteEncoding;

        constexpr static EncodingType kType = EncodingType::Concrete;
        constexpr static EncodingCharType kCharType = EncodingCharType::MultiByte;

        inline static CharT *Encode(uint codepoint, CharT *out)
        {
            auto it = out;
            if (codepoint < 0x80) // 10000000
            {
                it[0] = uchar(codepoint);

                return &it[1];
            }
            else if (codepoint < 0x800)
            {
                it[0] = ((codepoint >> 6) & 0x1F) | 0xC0;
                it[1] = (codepoint & 0x3F) | 0x80;

                return &it[2];
            }
            else if (codepoint < 0x10000)
            {
                it[0] = ((codepoint >> 12) & 0x0F) | 0xE0;
                it[1] = ((codepoint >> 6) & 0x3F) | 0x80;
                it[2] = ((codepoint >> 0) & 0x3F) | 0x80;

                return &it[3];
            }
            else if (codepoint < 0x10FFFF)
            {
                it[0] = ((codepoint >> 18) & 0x07) | 0xF0;
                it[1] = ((codepoint >> 12) & 0x3F) | 0x80;
                it[2] = ((codepoint >> 6) & 0x3F) | 0x80;
                it[3] = ((codepoint >> 0) & 0x3F) | 0x80;

                return &it[4];
            }
            else
            {
                throw UnsupportedEncodingException{};
            }
        }

        inline static const CharT *Decode(const CharT *begin, const CharT *end, uint &out)
        {
            auto it = (uchar *)begin;

            // uint32 codepoint;
            if (*it < 0x80) // 10000000
            {
                // 0-7F max
                // 1 byte

                out = it[0]; // 7 bit
                return (CharT *)&it[1];
            }
            else if (*it < 0xE0) // 11100000
            {
                // 7F-7FF
                // 2 byte

                if (it + 1 == (uchar *)end)
                {
                    // error invalid utf8 character
                    // character is out of range

                    throw CharacterOutOfRangeException{};
                }

                out = (ushort(it[0] & 0x1f) << 6) + (it[1] & 0x3F); // 11 bit
                return (CharT *)&it[2];
            }
            else if (*it < 0xF0) // 11110000
            {
                // 7FF-FFFF
                // 3 byte

                if (it + 2 >= (uchar *)end)
                {
                    // error invalid utf8 character
                    // character is out of range

                    throw CharacterOutOfRangeException{};
                }

                ushort codepoint = (ushort(it[0] & 0xF) << 12) + ((it[1] & 0x3F) << 6) + (it[2] & 0x3F); // 16 bit

                // surrogate check
                if (codepoint >= 0xD800 && codepoint <= 0xDFFF)
                {
                    // error invalid codepoint
                    // utf8 string contains surrogate codepoint

                    throw InvalidCodePointException{};
                }

                out = codepoint;

                return (CharT *)&it[3];
            }
            else
            {
                if (*it < 0xF8)
                {
                    // 10000-10FFFF
                    // 4 byte

                    if (it + 3 >= (uchar *)end)
                    {
                        // error invalid utf8 character
                        // character is out of range

                        throw CharacterOutOfRangeException{};
                    }

                    out = (uint(it[0] & 0x7) << 18) + ((it[1] & 0x3F) << 12) + ((it[2] & 0x3F) << 6) + (it[3] & 0x3F); // 21 bit

                    return (CharT *)&it[4];
                }
                else
                {
                    // error invalid first utf8 octet
                    // utf8 character length (> 4 bytes) is not supported

                    throw UnsupportedEncodingException{};
                }
            }
        }

        inline static const CharT* NextChar(const CharT *begin, const CharT *end)
        {
            auto it = (uchar *)begin;

            // uint32 codepoint;
            if (*it < 0x80) // 10000000
            {
                // 0-7F max
                // 1 byte

                return (CharT *)&it[1];
            }
            else if (*it < 0xE0) // 11100000
            {
                // 7F-7FF
                // 2 byte

                if (it + 1 == (uchar *)end)
                {
                    // error invalid utf8 character
                    // character is out of range

                    throw CharacterOutOfRangeException{};
                }

                return (CharT *)&it[2];
            }
            else if (*it < 0xF0) // 11110000
            {
                // 7FF-FFFF
                // 3 byte

                if (it + 2 >= (uchar *)end)
                {
                    // error invalid utf8 character
                    // character is out of range

                    throw CharacterOutOfRangeException{};
                }

                return (CharT *)&it[3];
            }
            else
            {
                if (*it < 0xF8)
                {
                    // 10000-10FFFF
                    // 4 byte

                    if (it + 3 >= (uchar *)end)
                    {
                        // error invalid utf8 character
                        // character is out of range

                        throw CharacterOutOfRangeException{};
                    }

                    return (CharT *)&it[4];
                }
                else
                {
                    // error invalid first utf8 octet
                    // utf8 character length (> 4 bytes) is not supported

                    throw UnsupportedEncodingException{};
                }
            }
        }
    };
}