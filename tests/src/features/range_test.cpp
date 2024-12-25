#include <gtest/gtest.h>
#include <ulib/range.h>

TEST(RangeTest, Enumeration)
{
    size_t index = 0;
    for (auto i : ulib::range(2600))
    {
        ASSERT_EQ(index, i);
        index++;
    }
}

TEST(RangeTest, EnumerationWithStartAndEnd)
{
    size_t index = 400;
    for (auto i : ulib::range(400, 2600))
    {
        ASSERT_EQ(index, i);
        index++;
    }
}