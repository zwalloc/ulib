#pragma once

#include <ulib/encodings/multibyte/encoding.h>
#include <ulib/encodings/utf16/encoding.h>
#include <ulib/encodings/utf32/encoding.h>
#include <ulib/encodings/utf8/encoding.h>
#include <ulib/encodings/wide/encoding.h>

#include <ulib/typetraits/field.h>

namespace ulib
{
    template <class T>
    struct literal_encoding
    {
        using type = ulib::missing_type;
    };

#ifdef __cpp_char8_t
    template <>
    struct literal_encoding<char>
    {
        using type = MultibyteEncoding;
    };

    template <>
    struct literal_encoding<char8_t>
    {
        using type = Utf8;
    };
#else
    template <>
    struct literal_encoding<char>
    {
        using type = Utf8;
    };
#endif

    template <>
    struct literal_encoding<char16_t>
    {
        using type = Utf16;
    };

    template <>
    struct literal_encoding<char32_t>
    {
        using type = Utf32;
    };

    template <>
    struct literal_encoding<wchar_t>
    {
        using type = WideEncoding;
    };

    ULIB_DEFINE_TYPE_CHECKS(literal_encoding);
} // namespace ulib