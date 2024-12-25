#pragma once

#include "encoding.h"
#include <ulib/containers/encodedstring.h>

namespace ulib
{
    using W1251StringSpan = EncodedStringSpan<W1251>;
    using W1251StringView = EncodedStringView<W1251>;
    using W1251String = EncodedString<W1251, DefaultAllocator>;

    using w1251string_span = W1251StringSpan;
    using w1251string_view = W1251StringView;
    using w1251string = W1251String;
}