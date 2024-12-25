#include <gtest/gtest.h>

#include <ulib/string.h>
#include <ulib/charcase.h>

#include <clocale>

TEST(StringTest, Charcase)
{
    std::setlocale(LC_ALL, ".utf8");

    ASSERT_EQ(ulib::upper(u8"test"), u8"TEST");
    ASSERT_EQ(ulib::lower(u8"TEST"), u8"test");
}
