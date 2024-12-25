#pragma once

#include <ulib/config.h>
#include <ulib/allocators/tempallocator.h>
#include <ulib/containers/encodedstring.h>
#include <ulib/typetraits/string.h>
#include <ulib/encodings/convert.h>
#include <ulib/encodings/wide/encoding.h>

#include <cwctype>

namespace ulib
{
    namespace detail
    {
        template <class AllocatorT>
        inline void UppercaseChars(EncodedString<WideEncoding, AllocatorT> &str)
        {
            for (auto &ch : str)
                ch = std::towupper(ch);
        }

        template <class AllocatorT>
        inline void LowercaseChars(EncodedString<WideEncoding, AllocatorT> &str)
        {
            for (auto &ch : str)
                ch = std::towlower(ch);
        }
    } // namespace detail

    template <class StringT, class EncodingT = argument_encoding_or_die_t<StringT>,
              class AllocatorT = constainer_choose_ulib_allocator_or_die_t<StringT>>
    inline EncodedString<EncodingT, AllocatorT> ToUpper(const StringT &str)
    {
        if constexpr (std::is_same_v<EncodingT, WideEncoding>)
        {
            auto wstr = Convert<WideEncoding, AllocatorT>(str);
            detail::UppercaseChars(wstr);
            return wstr;
        }
        else
        {
            auto wstr = Convert<WideEncoding, TempAllocatorT>(str);
            detail::UppercaseChars(wstr);
            return Convert<EncodingT, AllocatorT>(wstr);
        }
    }

    template <class StringT, class EncodingT = argument_encoding_or_die_t<StringT>,
              class AllocatorT = constainer_choose_ulib_allocator_or_die_t<StringT>>
    inline EncodedString<EncodingT, AllocatorT> ToLower(const StringT &str)
    {
        if constexpr (std::is_same_v<EncodingT, WideEncoding>)
        {
            auto wstr = Convert<WideEncoding, AllocatorT>(str);
            detail::LowercaseChars(wstr);
            return wstr;
        }
        else
        {
            auto wstr = Convert<WideEncoding, TempAllocatorT>(str);
            detail::LowercaseChars(wstr);
            return Convert<EncodingT, AllocatorT>(wstr);
        }
    }
} // namespace ulib