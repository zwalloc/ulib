#include <ulib/string.h>

#include <gtest/gtest.h>

TEST(EncodingsTest, SelfConvert)
{
    constexpr char8 kUtf8Str[] = u8"Hello world! 😎";
    constexpr char16 kUtf16Str[] = u"Hello world! 😎";
    constexpr char32 kUtf32Str[] = U"Hello world! 😎";
    constexpr wchar_t kWStr[] = L"Hello world! 😎";
    constexpr char kStr[] = "Hello world!";

    ASSERT_EQ(ulib::u8(kUtf8Str), kUtf8Str);
    ASSERT_EQ(ulib::u16(kUtf16Str), kUtf16Str);
    ASSERT_EQ(ulib::u32(kUtf32Str), kUtf32Str);
    ASSERT_EQ(ulib::wstr(kWStr), kWStr);
    ASSERT_EQ(ulib::str(kStr), kStr);

    ASSERT_EQ(ulib::u8(ulib::u8(kUtf8Str)), kUtf8Str);
    ASSERT_EQ(ulib::u16(ulib::u16(kUtf16Str)), kUtf16Str);
    ASSERT_EQ(ulib::u32(ulib::u32(kUtf32Str)), kUtf32Str);
    ASSERT_EQ(ulib::wstr(ulib::wstr(kWStr)), kWStr);
    ASSERT_EQ(ulib::str(ulib::str(kStr)), kStr);
}

TEST(EncodingsTest, MultipleUtfConvert)
{
    constexpr char8 kUtf8Str[] = u8"Hello world! 😎 чайник чайник, 传/傳, ぁすひぜちゆ, ÆÆÆ";
    constexpr char16 kUtf16Str[] = u"Hello world! 😎 чайник чайник, 传/傳, ぁすひぜちゆ, ÆÆÆ";
    constexpr char32 kUtf32Str[] = U"Hello world! 😎 чайник чайник, 传/傳, ぁすひぜちゆ, ÆÆÆ";
    constexpr wchar_t kWStr[] = L"Hello world! 😎 чайник чайник, 传/傳, ぁすひぜちゆ, ÆÆÆ";
    constexpr char kStr[] = "Hello world!";

    ASSERT_EQ(ulib::str(ulib::u8(ulib::u32(ulib::u16(ulib::u8(kStr))))), kStr);
    ASSERT_EQ(ulib::u8(ulib::u32(ulib::u16(ulib::u8(kUtf8Str)))), kUtf8Str);
    ASSERT_EQ(ulib::u16(ulib::u32(ulib::u16(ulib::u8(kUtf16Str)))), kUtf16Str);
    ASSERT_EQ(ulib::u32(ulib::u32(ulib::u16(ulib::u8(kUtf32Str)))), kUtf32Str);
    ASSERT_EQ(ulib::wstr(ulib::u32(ulib::u16(ulib::u8(kWStr)))), kWStr); 
}