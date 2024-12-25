#pragma once

namespace ulib
{
    namespace detail
    {
        typedef char u8char;
#ifdef _WIN32

        typedef wchar_t u16char;
        typedef char32_t u32char;
#else
        typedef char16_t u16char;
        typedef wchar_t u32char;
#endif
    }
}