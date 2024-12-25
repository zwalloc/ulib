#pragma once

#include "encoding.h"
#include <ulib/containers/encodedstring.h>

namespace ulib
{
    using PlatformStringSpan = EncodedStringSpan<PlatformEncoding>;
    using PlatformStringView = EncodedStringView<PlatformEncoding>;
    using PlatformString = EncodedString<PlatformEncoding, DefaultAllocator>;
    
    using plstring_span = PlatformStringSpan;
    using plstring_view = PlatformStringView;
    using plstring = PlatformString;
}