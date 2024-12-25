#include <gtest/gtest.h>
#include <ulib/strutility.h>
#include <ulib/string.h>

TEST(JoinTest, u8string)
{
    ulib::list<ulib::u8string> list{u8"one", u8"two", u8"three"};
    auto result = ulib::join(list, u8" | ");

    ASSERT_TRUE(result == u8"one | two | three");
}