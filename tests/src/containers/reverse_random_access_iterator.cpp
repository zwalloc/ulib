#include <gtest/gtest.h>
#include <ulib/containers/iterators/randomaccessiterator.h>
#include <ulib/containers/span.h>

TEST(ReverseRandomAccessIteratorTest, Enumeration)
{
    using value_type = char;
    using iterator = ulib::ReverseRandomAccessIterator<value_type>;

    value_type str[] = "narrator";

    {
        iterator begin = std::end(str) - 1;
        iterator end = std::begin(str) - 1;

        size_t i = (sizeof(str) / sizeof(value_type)) - 1;
        for (auto it = begin; it != end; it++)
        {
            ASSERT_EQ(*it, str[i]);
            i--;
        }
    }
}

TEST(ReverseRandomAccessIteratorTest, Compare)
{
    using value_type = char;
    using iterator = ulib::ReverseRandomAccessIterator<value_type>;

    value_type str[] = "narrator";

    {
        iterator begin = std::end(str) - 1;
        iterator end = std::begin(str) - 1;

        ASSERT_TRUE(begin < end);
        ASSERT_TRUE(end > begin);

        ASSERT_TRUE(begin <= end);
        ASSERT_TRUE(end >= begin);

        ASSERT_TRUE(begin != end);
    }
}

TEST(ReverseRandomAccessIteratorTest, Addition)
{
    using value_type = char;
    using iterator = ulib::ReverseRandomAccessIterator<value_type>;

    value_type str[] = "narrator";

    {
        iterator begin = std::end(str) - 1;

        ASSERT_EQ(*(begin + 0), '\0');
        ASSERT_EQ(*(begin + 1), 'r');
        ASSERT_EQ(*(begin + 2), 'o');
        ASSERT_EQ(*(begin + 3), 't');

        begin++;
        ASSERT_EQ(*begin, 'r');

        ++begin;
        ASSERT_EQ(*begin, 'o');
    }
}

TEST(ReverseRandomAccessIteratorTest, Subtraction)
{
    using value_type = char;
    using iterator = ulib::ReverseRandomAccessIterator<value_type>;

    value_type str[] = "narrator";

    {
        iterator end = std::begin(str) - 1;

        ASSERT_TRUE((end - 0) == end);
        ASSERT_EQ(*(end - 1), 'n');
        ASSERT_EQ(*(end - 2), 'a');
        ASSERT_EQ(*(end - 3), 'r');
        ASSERT_EQ(*(end - 4), 'r');

        end--;
        ASSERT_EQ(*end, 'n');

        --end;
        ASSERT_EQ(*end, 'a');      
    }
}

TEST(ReverseRandomAccessIteratorTest, Diff)
{
    using value_type = char;
    using iterator = ulib::ReverseRandomAccessIterator<value_type>;

    value_type str[] = "narrator";

    {
        iterator begin = std::end(str) - 1;
        iterator end = std::begin(str) - 1;

        ASSERT_EQ(end - begin, sizeof(str) / sizeof(value_type));
    }
}

TEST(ReverseRandomAccessIteratorTest, Base)
{
    using value_type = char;
    using iterator = ulib::ReverseRandomAccessIterator<value_type>;

    value_type str[] = "narrator";

    {
        iterator begin = std::end(str) - 1;
        iterator end = std::begin(str) - 1;

        ASSERT_TRUE(*begin.base() == '\0');
        ASSERT_TRUE(end.base().raw() == (str - 1));
    }
}