#pragma once

#include "encoding.h"
#include <ulib/containers/encodedstring.h>

namespace ulib
{
    using Utf16StringSpan = EncodedStringSpan<Utf16>;
    using Utf16StringView = EncodedStringView<Utf16>;
    using Utf16String = EncodedString<Utf16, DefaultAllocator>;

    using u16string_span = Utf16StringSpan;
    using u16string_view = Utf16StringView;
    using u16string = Utf16String;
} // namespace ulib