#pragma once

#include "encoding.h"
#include <ulib/containers/encodedstring.h>

namespace ulib
{
    using StringSpan = EncodedStringSpan<MultibyteEncoding>;
    using StringView = EncodedStringView<MultibyteEncoding>;
    using String = EncodedString<MultibyteEncoding, DefaultAllocator>;

    using string_span = StringSpan;
    using string_view = StringView;
    using string = String;
} // namespace ulib