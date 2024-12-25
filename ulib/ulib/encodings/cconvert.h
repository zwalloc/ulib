#pragma once

#include <type_traits>
#include <ulib/types.h>
#include <cstring>

#include "type.h"

namespace ulib
{
    template <class FromEncodingT, class ToEncodingT>
    typename ToEncodingT::CharT *ConvertChars(const typename FromEncodingT::CharT *begin, const typename FromEncodingT::CharT *end,
                                              typename ToEncodingT::CharT *output)
    {
        static_assert(FromEncodingT::kType != EncodingType::Raw, "It is not possible to convert raw encoding");
        static_assert(ToEncodingT::kType != EncodingType::Raw, "It is not possible to convert raw encoding");

        auto it = begin;
        auto out = output;

        while (it != end)
        {
            uint codepoint;

            it = FromEncodingT::Decode(it, end, codepoint);
            out = ToEncodingT::Encode(codepoint, out);
        }

        return out;

        /*
        if constexpr (std::is_same_v<FromEncodingT, ToEncodingT>)
        {
            size_t size = (uchar *)end - (uchar *)begin;
            memcpy(output, begin, size);
            return (typename ToEncodingT::CharT *)((uchar*)output + size);
        }
        else
        {
            static_assert(FromEncodingT::kType != EncodingType::Raw, "It is not possible to convert raw encoding");
            static_assert(ToEncodingT::kType != EncodingType::Raw, "It is not possible to convert raw encoding");

            auto it = begin;
            auto out = output;

            while (it != end)
            {
                uint codepoint;

                it = FromEncodingT::Decode(it, end, codepoint);
                out = ToEncodingT::Encode(codepoint, out);
            }

            return out;
        }
        */
    }
}