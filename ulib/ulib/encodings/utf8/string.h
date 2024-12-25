#pragma once

#include "encoding.h"
#include <ulib/containers/encodedstring.h>

namespace ulib
{
    using Utf8StringSpan = EncodedStringSpan<Utf8>;
    using Utf8StringView = EncodedStringView<Utf8>;
    using Utf8String = EncodedString<Utf8, DefaultAllocator>;

    using u8string_span = Utf8StringSpan;
    using u8string_view = Utf8StringView;
    using u8string = Utf8String;
}