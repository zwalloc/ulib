#include <gtest/gtest.h>
#include <numeric>

#include <ulib/list.h>
#include <ulib/string.h>

TEST(StringListTest, Pushing)
{
    ulib::List<ulib::u8string> list;
    ASSERT_EQ(list.Size(), 0);

    list.PushBack(u8"hello");
    list.PushBack(ulib::u8string_view(u8"hello"));
    list.PushBack(ulib::u8string(u8"hello"));

    ASSERT_EQ(list.Size(), 3);

    for (auto str : list)
    {
        ASSERT_EQ(str, u8"hello");
        ASSERT_EQ(str, ulib::u8string_view(u8"hello"));
        ASSERT_EQ(str, ulib::u8string(u8"hello"));
    }

    ASSERT_EQ(list.Size(), 3);
}

TEST(StringListTest, Moving)
{
    ulib::List<ulib::u8string> list;
    ASSERT_EQ(list.Size(), 0);

    list.PushBack(u8"hello");
    list.PushBack(ulib::u8string_view(u8"hello"));
    list.PushBack(ulib::u8string(u8"hello"));

    ASSERT_EQ(list.Size(), 3);

    auto moved = std::move(list);
    ASSERT_EQ(moved.Size(), 3);

    for (auto str : moved)
    {
        ASSERT_EQ(str, u8"hello");
        ASSERT_EQ(str, ulib::u8string_view(u8"hello"));
        ASSERT_EQ(str, ulib::u8string(u8"hello"));
    }

    ASSERT_EQ(moved.Size(), 3);

    auto moved2 = std::move(moved);
    ASSERT_EQ(moved2.Size(), 3);

    for (auto str : moved2)
    {
        ASSERT_EQ(str, u8"hello");
        ASSERT_EQ(str, ulib::u8string_view(u8"hello"));
        ASSERT_EQ(str, ulib::u8string(u8"hello"));
    }

    ASSERT_EQ(moved2.Size(), 3);

    ulib::List<ulib::u8string> moved3;

    {
        ulib::List<ulib::u8string> listIn;
        listIn.PushBack(u8"hello");
        listIn.PushBack(ulib::u8string_view(u8"hello"));
        listIn.PushBack(ulib::u8string(u8"hello"));

        moved3 = std::move(listIn);
    }

    ASSERT_EQ(moved3.Size(), 3);

    for (auto str : moved3)
    {
        ASSERT_EQ(str, u8"hello");
        ASSERT_EQ(str, ulib::u8string_view(u8"hello"));
        ASSERT_EQ(str, ulib::u8string(u8"hello"));
    }
}

TEST(StringListTest, Copying)
{
    ulib::List<ulib::u8string> list;

    list.PushBack(u8"hello");
    list.PushBack(ulib::u8string_view(u8"hello"));
    list.PushBack(ulib::u8string(u8"hello"));

    ulib::List<ulib::u8string> list2 = list;
    ASSERT_EQ(list.Size(), list2.Size());

    for (auto str : list2)
    {
        ASSERT_EQ(str, u8"hello");
        ASSERT_EQ(str, ulib::u8string_view(u8"hello"));
        ASSERT_EQ(str, ulib::u8string(u8"hello"));
    }

    ulib::List<ulib::u8string> list3;
    list3 = list2;

    ASSERT_EQ(list2.Size(), list3.Size());

    for (auto str : list3)
    {
        ASSERT_EQ(str, u8"hello");
        ASSERT_EQ(str, ulib::u8string_view(u8"hello"));
        ASSERT_EQ(str, ulib::u8string(u8"hello"));
    }

    ulib::List<ulib::u8string> list4;

    {
        ulib::List<ulib::u8string> listIn;
        listIn.PushBack(u8"hello");
        listIn.PushBack(ulib::u8string_view(u8"hello"));
        listIn.PushBack(ulib::u8string(u8"hello"));

        list4 = listIn;
    }

    for (auto str : list4)
    {
        ASSERT_EQ(str, u8"hello");
        ASSERT_EQ(str, ulib::u8string_view(u8"hello"));
        ASSERT_EQ(str, ulib::u8string(u8"hello"));
    }
}