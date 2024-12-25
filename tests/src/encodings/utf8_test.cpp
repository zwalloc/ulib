#include <ulib/encodings/utf8/encoding.h>
#include <ulib/encodings/utf16/encoding.h>
#include <ulib/encodings/utf32/encoding.h>

#include <ulib/encodings/convert.h>

#include <gtest/gtest.h>

TEST(Utf8Test, ToUtf16)
{
    constexpr char8 kUtf8Str[] = u8"Hello world! 😎";
    constexpr char16 kUtf16Str[] = u"Hello world! 😎";

    const auto converted = ulib::Convert<ulib::Utf16>(kUtf8Str);
    ASSERT_EQ(converted, kUtf16Str);
}

TEST(Utf8Test, ToUtf32)
{
    constexpr char8 kUtf8Str[] = u8"Hello world! 😎";
    constexpr char32 kUtf32Str[] = U"Hello world! 😎";

    const auto converted = ulib::Convert<ulib::Utf32>(kUtf8Str);    
    ASSERT_EQ(converted, kUtf32Str);
}