#pragma once

#include "../type.h"

#ifdef _WIN32
#include "../utf16/encoding.h"
namespace ulib
{
    using SystemWideEncoding = Utf16;
}
#else
#include "../utf32/encoding.h"
namespace ulib
{
    using SystemWideEncoding = Utf32;
}
#endif

namespace ulib
{
    namespace detail
    {
        using WideChar = typename ulib::SystemWideEncoding::CharT;
    }

    static_assert(sizeof(detail::WideChar) == sizeof(wchar_t), "[ulib error] wchar_t incompatible with ulib::SystemWideEncoding");

    struct WideEncoding
    {
        using CharT = wchar_t;

        constexpr static EncodingType kType = ulib::SystemWideEncoding::kType;
        constexpr static EncodingCharType kCharType = ulib::SystemWideEncoding::kCharType;

        inline static CharT *Encode(uint codepoint, CharT *out)
        {
            return (CharT *)ulib::SystemWideEncoding::Encode(codepoint, (detail::WideChar *)out);
        }

        inline static const CharT *Decode(const CharT *begin, const CharT *end, uint &out)
        {
            return (CharT *)ulib::SystemWideEncoding::Decode((detail::WideChar *)begin, (detail::WideChar *)end, out);
        }

#ifdef _WIN32
        inline static const CharT *NextChar(const CharT *begin, const CharT *end)
        {
            return (CharT *)ulib::SystemWideEncoding::NextChar((detail::WideChar *)begin, (detail::WideChar *)end);
        }
#endif
    };
}