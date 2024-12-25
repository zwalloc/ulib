#pragma once

#include "encoding.h"
#include <ulib/containers/encodedstring.h>

namespace ulib
{
    using WideStringSpan = EncodedStringSpan<WideEncoding>;
    using WideStringView = EncodedStringView<WideEncoding>;
    using WideString = EncodedString<WideEncoding, DefaultAllocator>;

    using wstring_span = WideStringSpan;
    using wstring_view = WideStringView;
    using wstring = WideString;
}