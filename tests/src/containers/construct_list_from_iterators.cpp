#include <gtest/gtest.h>

#include <ulib/list.h>
#include <ulib/split.h>
#include <ulib/string.h>

TEST(ListTest, FromSplit)
{
    for (size_t i = 0; i != 2000; i++)
    {
        auto spl = ulib::split("hello world hello", " ");
        ulib::list<ulib::string> list{spl.begin(), spl.end()};

        ASSERT_EQ(list.size(), 3);
        ASSERT_TRUE(list[0] == "hello");
        ASSERT_TRUE(list[1] == "world");
        ASSERT_TRUE(list[2] == "hello");
    }

    for (size_t i = 0; i != 2000; i++)
    {
        auto spl = ulib::split("hello world hello", " ");
        ulib::list<std::string> list{spl.begin() + 1, spl.end()};

        ASSERT_EQ(list.size(), 2);
        ASSERT_TRUE(list[0] == "world");
        ASSERT_TRUE(list[1] == "hello");
    }

    for (size_t i = 0; i != 2000; i++)
    {
        auto spl = ulib::split("hello world hello", " ");
        ulib::list<std::string> list{spl.begin() + 2, spl.end()};

        ASSERT_EQ(list.size(), 1);
        ASSERT_TRUE(list[0] == "hello");
    }

    for (size_t i = 0; i != 2000; i++)
    {
        auto spl = ulib::split("hello world hello", " ");
        ulib::list<std::string> list{spl.begin() + 3, spl.end()};

        ASSERT_EQ(list.size(), 0);
    }
}

TEST(ListTest, FromList)
{
    for (size_t i = 0; i != 2000; i++)
    {
        ulib::list<ulib::string> spl{"hello", "world", "hello"};
        ulib::list<ulib::string> list{spl.begin(), spl.end()};

        ASSERT_EQ(list.size(), 3);
        ASSERT_TRUE(list[0] == "hello");
        ASSERT_TRUE(list[1] == "world");
        ASSERT_TRUE(list[2] == "hello");
    }

    for (size_t i = 0; i != 2000; i++)
    {
        ulib::list<ulib::string> spl{"hello", "world", "hello"};
        ulib::list<ulib::string> list{spl.begin() + 1, spl.end() - 1};

        ASSERT_EQ(list.size(), 1);
        ASSERT_TRUE(list[0] == "world");
    }

    for (size_t i = 0; i != 2000; i++)
    {
        ulib::list<std::string> spl{"hello", "world", "hello"};
        ulib::list<ulib::string> list{spl.begin() + 2, spl.end()};

        ASSERT_EQ(list.size(), 1);
    }
}

TEST(ListTest, FromVector)
{
    for (size_t i = 0; i != 2000; i++)
    {
        std::vector<ulib::string> spl{"hello", "world", "hello"};
        ulib::list<ulib::string> list{spl.begin(), spl.end()};

        ASSERT_EQ(list.size(), 3);
        ASSERT_TRUE(list[0] == "hello");
        ASSERT_TRUE(list[1] == "world");
        ASSERT_TRUE(list[2] == "hello");
    }

    for (size_t i = 0; i != 2000; i++)
    {
        std::vector<std::string> spl{"hello", "world", "hello"};
        ulib::list<ulib::string> list{spl.begin(), spl.end()};

        ASSERT_EQ(list.size(), 3);
        ASSERT_TRUE(list[0] == "hello");
        ASSERT_TRUE(list[1] == "world");
        ASSERT_TRUE(list[2] == "hello");
    }
}