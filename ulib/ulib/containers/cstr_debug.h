#pragma once

#ifdef _WIN32
#ifdef _DEBUG

#include <stdint.h>

namespace ulib
{
    const char* c_str_read_check(const char* str);
    const uint16_t* c_str_read_check(const uint16_t* str);
    const uint32_t* c_str_read_check(const uint32_t* str);

    template <class CharT>
    const CharT *c_str_read_check_tpl(const CharT *ch)
    {
        if constexpr (sizeof(CharT) == sizeof(char))
        {
            return (CharT*)c_str_read_check((char*)ch);
        }
        else if constexpr (sizeof(CharT) == sizeof(uint16_t))
        {
            return (CharT*)c_str_read_check((uint16_t*)ch);
        }
        else if constexpr (sizeof(CharT) == sizeof(uint32_t))
        {
            return (CharT*)c_str_read_check((uint32_t*)ch);
        }
        else 
        {
            return ch;
        }
    }
}

#endif
#endif