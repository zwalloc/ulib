#pragma once

#include "encoding.h"
#include <ulib/containers/encodedstring.h>

namespace ulib
{
    using Utf32StringSpan = EncodedStringSpan<Utf32>;
    using Utf32StringView = EncodedStringView<Utf32>;
    using Utf32String = EncodedString<Utf32, DefaultAllocator>;

    using u32string_span = Utf32StringSpan;
    using u32string_view = Utf32StringView;
    using u32string = Utf32String;
} // namespace ulib