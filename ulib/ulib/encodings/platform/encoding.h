#pragma once

#ifdef _WIN32
    #include "../utf16/encoding.h"
#else
    #include "../utf8/encoding.h"
#endif

// #include <ulib/containers/string.h>

namespace ulib
{
#ifdef _WIN32
    using PlatformEncoding = Utf16;
#else
    using PlatformEncoding = Utf8;
#endif
}