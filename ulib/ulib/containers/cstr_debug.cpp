#ifdef _WIN32
#ifdef _DEBUG

#include "cstr_debug.h"

#include <assert.h>
#include <windows.h>

namespace ulib
{
    const char *c_str_read_check(const char *str)
    {
        assert(
            !IsBadStringPtrA(str, 0xFFFFFFFF) &&
            "Invalid C-String Detected in ulib::c_str_read_check(const char*). It may be reason of memory leaking or other bad string conversions.");
        return str;
    }

    const uint16_t *c_str_read_check(const uint16_t *str)
    {
        assert(!IsBadStringPtrW((wchar_t *)str, 0xFFFFFFFF) &&
               "Invalid C-String Detected in ulib::c_str_read_check(const wchar_t*). It may be reason of memory "
               "leaking or other bad string conversions.");
        return str;
    }

    const uint32_t *c_str_read_check(const uint32_t *str)
    {
        auto it = str;
        while (true)
        {
            assert(!IsBadReadPtr(it, sizeof(uint32_t)) && "Invalid C-String Detected in ulib::c_str_read_check(const uint32_t*). It may be reason of "
                                                          "memory leaking or other bad string conversions.");

            if (*it == 0)
                break;

            it++;
        }

        return str;
    }
} // namespace ulib

#endif
#endif