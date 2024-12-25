#pragma once

#include <type_traits>
#include <ulib/typetraits/field.h>

namespace ulib
{
    enum class EncodingType
    {
        Raw = 0,
        Concrete = 1,
    };

    enum class EncodingCharType
    {
        SingleByte = 0,
        MultiByte = 1,
    };

    ULIB_DEFINE_TYPE_FIELD_CHECK(encoding_char, CharT);
    ULIB_DEFINE_TYPE_FIELD_CHECK(parent_encoding, ParentEncodingT);

    template <class SourceEncodingT, class EncodingT>
    inline constexpr bool is_encodings_raw_movable_v =
        (std::is_same_v<SourceEncodingT, parent_encoding_t<EncodingT>> ||
        std::is_same_v<parent_encoding_t<SourceEncodingT>, EncodingT> ||
        std::is_same_v<SourceEncodingT, EncodingT>) && !std::is_same_v<SourceEncodingT, missing_type>;
}