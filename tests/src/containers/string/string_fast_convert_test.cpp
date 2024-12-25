#include <gtest/gtest.h>
#include <ulib/string.h>

TEST(StringTest, FastConvert)
{
    auto result = ulib::str(ulib::u8(ulib::u16(ulib::u32(ulib::u8(ulib::str("hello guys"))))));
    ASSERT_EQ(result, "hello guys");
}
