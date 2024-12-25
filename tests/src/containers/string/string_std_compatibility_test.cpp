#include <gtest/gtest.h>
#include <ulib/string.h>

#ifdef __cpp_char8_t
using std_u8string = std::u8string;
#else
using std_u8string = std::string;
#endif

TEST(StringStdCompatibilityTest, Equals)
{

    std::string std_raw_str = "hello";
    std_u8string std_u8_str = u8"hello";
    std::u16string std_u16_str = u"hello";
    std::u32string std_u32_str = U"hello";
    std::wstring std_w_str = L"hello";

    ulib::string raw_str = "hello";
    ulib::u8string u8_str = u8"hello";
    ulib::u16string u16_str = u"hello";
    ulib::u32string u32_str = U"hello";
    ulib::wstring w_str = L"hello";

    ASSERT_TRUE(std_raw_str == raw_str);
    ASSERT_TRUE(std_u8_str == u8_str);
    ASSERT_TRUE(std_u16_str == u16_str);
    ASSERT_TRUE(std_u32_str == u32_str);
    ASSERT_TRUE(std_w_str == w_str);

    ASSERT_TRUE(raw_str == std_raw_str);
    ASSERT_TRUE(u8_str == std_u8_str);
    ASSERT_TRUE(u16_str == std_u16_str);
    ASSERT_TRUE(u32_str == std_u32_str);
    ASSERT_TRUE(w_str == std_w_str);
}

TEST(StringStdCompatibilityTest, NotEquals)
{
    std::string std_raw_str = "hello";
    std_u8string std_u8_str = u8"hello";
    std::u16string std_u16_str = u"hello";
    std::u32string std_u32_str = U"hello";
    std::wstring std_w_str = L"hello";

    ulib::string raw_str = "hello1";
    ulib::u8string u8_str = u8"hello1";
    ulib::u16string u16_str = u"hello1";
    ulib::u32string u32_str = U"hello1";
    ulib::wstring w_str = L"hello1";

    ASSERT_TRUE(std_raw_str != raw_str);
    ASSERT_TRUE(std_u8_str != u8_str);
    ASSERT_TRUE(std_u16_str != u16_str);
    ASSERT_TRUE(std_u32_str != u32_str);
    ASSERT_TRUE(std_w_str != w_str);

    ASSERT_TRUE(raw_str != std_raw_str);
    ASSERT_TRUE(u8_str != std_u8_str);
    ASSERT_TRUE(u16_str != std_u16_str);
    ASSERT_TRUE(u32_str != std_u32_str);
    ASSERT_TRUE(w_str != std_w_str);
}