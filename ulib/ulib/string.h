#pragma once

#include <ulib/config.h>

#include <ulib/encodings/multibyte/string.h>
#include <ulib/encodings/utf16/string.h>
#include <ulib/encodings/utf32/string.h>
#include <ulib/encodings/utf8/string.h>
#include <ulib/encodings/wide/string.h>
#include <ulib/encodings/convert.h>

#include <ulib/typetraits/string.h>

namespace ulib
{
    template <class EncodingT, class AllocatorT, std::enable_if_t<is_encodings_raw_movable_v<EncodingT, MultibyteEncoding>, bool> = true>
    inline EncodedString<MultibyteEncoding, AllocatorT> str(EncodedString<EncodingT, AllocatorT> &&str)
    {
        return Convert<MultibyteEncoding>(std::move(str));
    }
    template <class EncodingT, class AllocatorT, std::enable_if_t<is_encodings_raw_movable_v<EncodingT, WideEncoding>, bool> = true>
    inline EncodedString<WideEncoding, AllocatorT> wstr(EncodedString<EncodingT, AllocatorT> &&str)
    {
        return Convert<WideEncoding>(std::move(str));
    }

    template <class EncodingT, class AllocatorT, std::enable_if_t<is_encodings_raw_movable_v<EncodingT, Utf8>, bool> = true>
    inline EncodedString<Utf8, AllocatorT> u8(EncodedString<EncodingT, AllocatorT> &&str)
    {
        return Convert<Utf8>(std::move(str));
    }

    template <class EncodingT, class AllocatorT, std::enable_if_t<is_encodings_raw_movable_v<EncodingT, Utf16>, bool> = true>
    inline EncodedString<Utf16, AllocatorT> u16(EncodedString<EncodingT, AllocatorT> &&str)
    {
        return Convert<Utf16>(std::move(str));
    }

    template <class EncodingT, class AllocatorT, std::enable_if_t<is_encodings_raw_movable_v<EncodingT, Utf32>, bool> = true>
    inline EncodedString<Utf32, AllocatorT> u32(EncodedString<EncodingT, AllocatorT> &&str)
    {
        return Convert<Utf32>(std::move(str));
    }

    template <class StringT, class EncodingT = argument_encoding_or_die_t<StringT>>
    inline string str(const StringT &str)
    {
        return Convert<MultibyteEncoding>(str);
    }

    template <class StringT, class EncodingT = argument_encoding_or_die_t<StringT>>
    inline wstring wstr(const StringT &str)
    {
        return Convert<WideEncoding>(str);
    }

    template <class StringT, class EncodingT = argument_encoding_or_die_t<StringT>>
    inline u8string u8(const StringT &str)
    {
        return Convert<Utf8>(str);
    }

    template <class StringT, class EncodingT = argument_encoding_or_die_t<StringT>>
    inline u16string u16(const StringT &str)
    {
        return Convert<Utf16>(str);
    }

    template <class StringT, class EncodingT = argument_encoding_or_die_t<StringT>>
    inline u32string u32(const StringT &str)
    {
        return Convert<Utf32>(str);
    }

#ifdef ULIB_STD_COMPATIBILITY

    template <class StringT, class EncodingT = argument_encoding_or_die_t<StringT>>
    inline std::string sstr(const StringT &str)
    {
        return u8(str);
    }

    template <class StringT, class EncodingT = argument_encoding_or_die_t<StringT>>
    inline std::wstring swstr(const StringT &str)
    {
        return wstr(u8(str));
    }

#endif
} // namespace ulib