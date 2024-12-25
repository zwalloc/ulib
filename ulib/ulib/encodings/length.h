#pragma once

#include "type.h"

namespace ulib
{
    template <class EncodingT>
    size_t CalcStringLength(const typename EncodingT::CharT *begin, const typename EncodingT::CharT *end)
    {
        if constexpr (EncodingT::kCharType == EncodingCharType::SingleByte)
        {
            return size_t(end - begin);
        }
        else
        {
            size_t length = 0;
            auto it = begin;
            while (it != end)
            {
                ++length;
                it = EncodingT::NextChar(it, end);          
            }

            return length;
        }
    }
}