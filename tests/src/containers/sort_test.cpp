#include <array>
#include <gtest/gtest.h>
#include <numeric>
#include <ulib/list.h>
#include <ulib/range.h>
#include <ulib/string.h>

TEST(ListSortTest, IntSort)
{
    ulib::list<int> list = {2, 5, 8, 1};

    std::sort(list.begin(), list.end(), [](auto left, auto right) { return left < right; });

    ASSERT_TRUE(list.size() == 4);
    ASSERT_TRUE(list == ulib::list<int>({1, 2, 5, 8}));

    std::sort(list.begin(), list.end(), [](auto left, auto right) { return left > right; });
    ASSERT_TRUE(list == ulib::list<int>({8, 5, 2, 1}));
}

TEST(ListSortTest, StringSort)
{
    ulib::list<ulib::string> list = {"abc", "def", "123", "456"};
    std::sort(list.begin(), list.end(), [](auto left, auto right) { return left < right; });

    ASSERT_TRUE(list.size() == 4);
    ASSERT_TRUE(list == ulib::list<ulib::string>({"123", "456", "abc", "def"}));

    std::sort(list.begin(), list.end(), [](auto left, auto right) { return left > right; });
    ASSERT_TRUE(list == ulib::list<ulib::string>({"def", "abc", "456", "123"}));
}

TEST(ListSortTest, ListStringSort)
{
    ulib::list<ulib::list<ulib::string>> list = {
        {"abc", "def"}, {"abc", "def", "123", "456", "789"}, {"abc", "def", "123", "456"}, {"abc", "def", "123"}};

    std::sort(list.begin(), list.end(), [](auto left, auto right) { return left.size() < right.size(); });

    ASSERT_TRUE(list.size() == 4);
    ASSERT_TRUE(list[0].size() == 2);
    ASSERT_TRUE(list[1].size() == 3);
    ASSERT_TRUE(list[2].size() == 4);
    ASSERT_TRUE(list[3].size() == 5);


    std::sort(list.begin(), list.end(), [](auto left, auto right) { return left.size() > right.size(); });

    ASSERT_TRUE(list.size() == 4);
    ASSERT_TRUE(list[0].size() == 5);
    ASSERT_TRUE(list[1].size() == 4);
    ASSERT_TRUE(list[2].size() == 3);
    ASSERT_TRUE(list[3].size() == 2);
}