#pragma once

#include "../exceptions.h"
#include "../multibyte/encoding.h"
#include "../type.h"
#include "../uchars.h"
#include <ulib/types.h>

namespace ulib
{
    struct W1251
    {
        using CharT = char;
        using ParentEncodingT = MultibyteEncoding;

        constexpr static EncodingType kType = EncodingType::Concrete;
        constexpr static EncodingCharType kCharType = EncodingCharType::SingleByte;

        class Table
        {
        public:
            Table() { MakeTable(); }

            inline uint Decode(uchar ch)
            {
                uint codepoint = mData[ch];

                const uint kEmpty = 0xFFFFFFFF;
                if (codepoint == kEmpty)
                    throw UnsupportedEncodingException{};

                return codepoint;
            }

            inline uchar Encode(uint codepoint)
            {
                // ascii codes
                if (codepoint < 128)
                    return uchar(codepoint);

                // russian letters
                if (codepoint < 0x410 + 64 && codepoint >= 0x410)
                    return uchar(codepoint - 0x350);

                // other data
                for (uchar i = 128; i != 192; i++)
                {
                    if (mData[i] == codepoint)
                    {
                        const uint kEmpty = 0xFFFFFFFF;
                        if (mData[i] == kEmpty)
                            throw UnsupportedEncodingException{};

                        return uchar(i);
                    }
                }

                throw UnsupportedEncodingException{};
            }

        private:
            uint mData[256];

            void MakeTable()
            {
                for (uint i = 0; i != 128; i++)
                    mData[i] = i;

                const uint kEmpty = 0xFFFFFFFF;

                // t[128] = 0x402;
                uint partTwo[64] = {0x402,  0x403,  0x201A, 0x0453, 0x201E, 0x2026, 0x2020, 0x2021, 0x20AC, 0x2030, 0x0409, 0x2039, 0x040a,
                                    0x040c, 0x040b, 0x040f, 0x0452, 0x2018, 0x2019, 0x201c, 0x201d, 0x2022, 0x2013, 0x2014, kEmpty, 0x2122,
                                    0x0459, 0x203A, 0x045a, 0x45c,  0x45b,  0x45f,  0xa0,   0x40e,  0x45e,  0x408,  0xa4,   0x490,  0xa6,
                                    0xa7,   0x401,  0xa9,   0x404,  0xab,   0xac,   0xad,   0xae,   0x407,  0xb0,   0xb1,   0x406,  0x456,
                                    0x491,  0xb5,   0xb6,   0xb7,   0x451,  0x2116, 0x454,  0xbb,   0x458,  0x405,  0x455,  0x457};

                uint partThree[64];
                for (uint i = 0; i != 64; i++)
                {
                    partThree[i] = 0x410 + i;
                }

                memcpy(mData + 128, partTwo, 64 * sizeof(uint));
                memcpy(mData + 128 + 64, partThree, 64 * sizeof(uint));
            }
        };

        
        inline static CharT *Encode(uint codepoint, CharT *out)
        {
            static Table codepage;

            auto it = out;
            uchar ch = codepage.Encode(codepoint);
            it[0] = CharT(ch);

            return &it[1];
        }

        inline static const CharT *Decode(const CharT *begin, const CharT *end, uint &out)
        {
            static Table codepage;

            auto it = (uchar *)begin;
            out = codepage.Decode(*it);

            return (CharT *)&it[1];
        }

        inline static const CharT *NextChar(const CharT *begin, const CharT *end)
        {
            auto it = (uchar *)begin;
            return (CharT *)&it[1];
        }
    };


    using w1251 = W1251;
} // namespace ulib