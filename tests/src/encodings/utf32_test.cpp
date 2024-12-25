#include <ulib/encodings/utf8/encoding.h>
#include <ulib/encodings/utf16/encoding.h>
#include <ulib/encodings/utf32/encoding.h>

#include <ulib/encodings/convert.h>

#include <gtest/gtest.h>

TEST(Utf32Test, ToUtf8)
{
    constexpr char32 kUtf32Str[] = U"Hello world! 😎";
    constexpr char8 kUtf8Str[] = u8"Hello world! 😎";

    const auto converted = ulib::Convert<ulib::Utf8>(kUtf32Str);    
    ASSERT_EQ(converted, kUtf8Str);
}

TEST(Utf32Test, ToUtf16)
{
    constexpr char32 kUtf32Str[] = U"Hello world! 😎";
    constexpr char16 kUtf16Str[] = u"Hello world! 😎";

    const auto converted = ulib::Convert<ulib::Utf16>(kUtf32Str);    
    ASSERT_EQ(converted, kUtf16Str);
}