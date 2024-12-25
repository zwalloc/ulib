#include <ulib/types.h>
#include <gtest/gtest.h>

TEST(CoreTypesTest, IntegerSizesAreCorrect)
{
    ASSERT_GE(sizeof(ushort), sizeof(uchar));
    ASSERT_GE(sizeof(uint), sizeof(ushort));
    ASSERT_GE(sizeof(ulong), sizeof(uint));
    ASSERT_GE(sizeof(ullong), sizeof(ulong));
}