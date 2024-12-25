#pragma once

#include <ulib/containers/encodedstring.h>
#include <ulib/containers/encodedstringview.h>
#include <ulib/typetraits/literalencoding.h>
#include <ulib/typetraits/typeordefault.h>

#include "cconvert.h"
#include "type.h"

namespace ulib
{
    template <class FromEncodingT, class ToEncodingT, class AllocatorT = DefaultAllocator>
    inline EncodedString<ToEncodingT, AllocatorT> ConvertImpl(EncodedStringView<FromEncodingT> str, typename AllocatorT::Params al = {})
    {
        static_assert(FromEncodingT::kType != EncodingType::Raw, "It is not possible to convert raw encoding");
        static_assert(ToEncodingT::kType != EncodingType::Raw, "It is not possible to convert raw encoding");

        EncodedString<ToEncodingT, AllocatorT> result(str.size() * 4, al);

        auto end = ulib::ConvertChars<FromEncodingT, ToEncodingT>(str.data(), str.data() + str.size(), result.data());
        result.resize(end - result.data());

        return result;
    }

    template <class UOutputEncodingT = missing_type, class EncodingT, class AllocatorT,
              class OutputEncodingT = type_or_default_t<UOutputEncodingT, EncodingT>,
              std::enable_if_t<is_encodings_raw_movable_v<EncodingT, OutputEncodingT>, bool> = true>
    inline EncodedString<OutputEncodingT, AllocatorT> Convert(EncodedString<EncodingT, AllocatorT> &&str)
    {
        // here utf8 and multibyte are dances with flaming glow
        using DestCharT = typename OutputEncodingT::CharT;
        static_assert(sizeof(typename EncodingT::CharT) == sizeof(DestCharT),
                      "Parent encoding character size must be equal with base encoding to compatibile convertation");

        // EncodedString<OutputEncodingT, AllocatorT> result = std::move(*(EncodedString<OutputEncodingT, AllocatorT> *)&str);
        return std::move(*(EncodedString<OutputEncodingT, AllocatorT> *)&str);
    }

    template <class UOutputEncodingT = missing_type, class UOutputAllocatorT = missing_type, class StringT,
              class EncodingT = argument_encoding_or_die_t<StringT>, class AllocatorT = constainer_choose_ulib_allocator_or_die_t<StringT>,
              class OutputEncodingT = type_or_default_t<UOutputEncodingT, EncodingT>,
              class OutputAllocatorT = type_or_default_t<UOutputAllocatorT, AllocatorT>>
    inline EncodedString<OutputEncodingT, OutputAllocatorT> Convert(const StringT &str, typename OutputAllocatorT::Params al = {})
    {
        EncodedStringView<EncodingT> view = str;
        if constexpr (is_encodings_raw_movable_v<EncodingT, OutputEncodingT>)
        {
            // here utf8 and multibyte are dances with flaming glow
            using DestCharT = typename OutputEncodingT::CharT;
            static_assert(sizeof(typename EncodingT::CharT) == sizeof(DestCharT),
                          "Parent encoding character size must be equal with base encoding to compatibile convertation");

            return EncodedString<OutputEncodingT, OutputAllocatorT>((DestCharT *)view.data(), (DestCharT *)view.data() + view.size(), al);
        }
        else
        {
            return ConvertImpl<EncodingT, OutputEncodingT, OutputAllocatorT>(view, al);
        }
    }

} // namespace ulib