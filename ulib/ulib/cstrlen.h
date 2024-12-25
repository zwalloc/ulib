#pragma once

#include <cstddef>
#include <type_traits>

namespace ulib
{

    template <class CharT>
    constexpr size_t cstrlen(const CharT *ptr) noexcept
    {

        if constexpr (std::is_same_v<CharT, char>
#ifdef __cpp_char8_t
                      || std::is_same_v<CharT, char8_t>

#endif
        )
        {
            return __builtin_strlen((const char *)ptr);
        }
        else
        {
            size_t len = 0;
            while (*ptr != 0)
                ++len, ++ptr;
            return len;
        }
    }
} // namespace ulib