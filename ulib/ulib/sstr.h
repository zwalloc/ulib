#pragma once

#include <ulib/u8.h>
#include <ulib/wchar.h>

namespace ulib
{

#ifdef ULIB_STD_COMPATIBILITY

    template<class StringT, class EncodingT = argument_encoding_or_die_t<StringT>>
    inline std::string sstr(const StringT& str)
    {
        return u8(str);
    }

    template<class StringT, class EncodingT = argument_encoding_or_die_t<StringT>>
    inline std::wstring swstr(const StringT &str)
    {
        return wstr(u8(str));
    }

#endif

}