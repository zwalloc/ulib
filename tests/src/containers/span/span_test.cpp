#include <gtest/gtest.h>

#include <ulib/span.h>

TEST(SpanTest, Empty)
{
    {
        ulib::span<const int> ints;
        ASSERT_TRUE(ints.empty());
    }

    {
        int buf[5] = {0, 1, 2, 3, 4};
        ulib::span<const int> ints(buf);
        ASSERT_FALSE(ints.empty());

        
    }

    {
        ulib::span<int> ints;
        ASSERT_TRUE(ints.empty());
    }

    {
        int buf[5] = {0, 1, 2, 3, 4};
        ulib::span<int> ints(buf);
        ASSERT_FALSE(ints.empty());
    }
}

TEST(SpanTest, Sizing)
{
    {
        ulib::span<int> ints;
        ASSERT_EQ(ints.size(), 0);
        ASSERT_EQ(ints.size_in_bytes(), 0);
    }

    {
        int buf[5] = {0, 1, 2, 3, 4};
        ulib::span<int> ints(buf);
        ASSERT_EQ(ints.size(), 5);
        ASSERT_EQ(ints.size_in_bytes(), 5 * sizeof(int));
    }
}

TEST(SpanTest, FromArray)
{
    int buf[5] = {0, 1, 2, 3, 4};
    ulib::span<int> ints(buf);

    int index = 0;
    for (auto obj : ints)
    {
        ASSERT_EQ(index, obj);
        index++;
    }
}

TEST(SpanTest, Data)
{
    {
        ulib::span<int> ints;
        ASSERT_EQ(ints.data(), nullptr);
    }

    {
        int buf[5] = {0, 1, 2, 3, 4};
        ulib::span<int> ints(buf);
        ASSERT_EQ((size_t)ints.data(), (size_t)&buf);
    }
}

TEST(SpanTest, Compare)
{
    {
        ulib::span<int> ints;
        ulib::span<int> ints2;

        ASSERT_TRUE(ints.compare(ints2));
        ASSERT_EQ(ints, ints2);
    }

    {
        int buf[5] = {0, 1, 2, 3, 4};
        ulib::span<int> ints(buf);
        ulib::span<int> ints2(buf);

        ASSERT_TRUE(ints.compare(ints2));
        ASSERT_EQ(ints, ints2);
    }

    {
        int buf[5] = {0, 1, 2, 3, 4};
        ulib::span<int> ints(buf);

        int buf2[5] = {1, 2, 3};
        ulib::span<int> ints2(buf2);

        ASSERT_FALSE(ints.compare(ints2));
        ASSERT_NE(ints, ints2);
    }

    {
        int buf[5] = {0, 1, 2, 3, 4};
        ulib::span<int> ints(buf);

        int buf2[5] = {0, 1, 2, 3};
        ulib::span<int> ints2(buf2);

        ASSERT_FALSE(ints.compare(ints2));
        ASSERT_NE(ints, ints2);
    }

    {
        int buf[5] = {0, 1, 2, 3, 4};
        ulib::span<int> ints(buf);

        int buf2[5] = {3, 4};
        ulib::span<int> ints2(buf2);

        ASSERT_FALSE(ints.compare(ints2));
        ASSERT_NE(ints, ints2);
    }
}

TEST(SpanTest, FrontAndBack)
{
    {
        int buf[5] = {0, 1, 2, 3, 4};
        ulib::span<int> ints(buf);

        ASSERT_EQ(ints.front(), 0);
        ASSERT_EQ(ints.back(), 4);
    }
}

TEST(SpanTest, StartsWith)
{
    {
        int buf[5] = {0, 1, 2, 3, 4};
        ulib::span<int> ints(buf);

        int buf2[3] = {0, 1, 2};
        ulib::span<int> ints2(buf2);

        int buf3[3] = {4, 1, 2};
        ulib::span<int> ints3(buf3);

        ASSERT_TRUE(ints.starts_with(0));
        ASSERT_FALSE(ints.starts_with(1));

        ASSERT_TRUE(ints.starts_with(ints2));
        ASSERT_FALSE(ints.starts_with(ints3));
    }
}

TEST(SpanTest, EndsWith)
{
    {
        int buf[5] = {0, 1, 2, 3, 4};
        ulib::span<int> ints(buf);

        int buf2[3] = {2, 3, 4};
        ulib::span<int> ints2(buf2);

        int buf3[3] = {4, 1, 2};
        ulib::span<int> ints3(buf3);

        ASSERT_TRUE(ints.ends_with(4));
        ASSERT_FALSE(ints.ends_with(1));

        ASSERT_TRUE(ints.ends_with(ints2));
        ASSERT_FALSE(ints.ends_with(ints3));
    }
}

TEST(SpanTest, Find)
{
    int buf[5] = {0, 1, 2, 3, 4};
    ulib::span<int> ints(buf);

    int buf2[3] = {2, 3, 4};
    ulib::span<int> ints2(buf2);

    int buf3[3] = {4, 1, 2};
    ulib::span<int> ints3(buf3);

    ASSERT_EQ(ints.find(3), 3);
    ASSERT_EQ(ints.find(20), ulib::npos);
    ASSERT_EQ(ints.find(ints2), 2);
}

TEST(SpanTest, ReverseFind)
{
    {
        char str[] = {'a', 'b', 'c', '!', 'a', 'b', 'c', '!', 'a', 'b', 'c'};
        char fstr[] = {'a', 'b', 'c'};
        char fstre[] = {'!', 'a', 'b', 'c'};
        char fstrf[] = {'!', 'a', 'b'};

        ulib::span<char> spn(str);
        ASSERT_EQ(spn.rfind('!'), 7);
        ASSERT_EQ(spn.rfind('a'), 8);
        ASSERT_EQ(spn.rfind('b'), 9);
        ASSERT_EQ(spn.rfind('c'), 10);

        ulib::span<char> fspn(fstr);
        ASSERT_EQ(fspn.rfind('a'), 0);
        ASSERT_EQ(fspn.rfind('b'), 1);
        ASSERT_EQ(fspn.rfind('c'), 2);

        ASSERT_EQ(spn.rfind(fstr), 8);
        ASSERT_EQ(spn.rfind(fstre), 7);
        ASSERT_EQ(spn.rfind(fstrf), 7);
        ASSERT_EQ(spn.rfind(str), 0);
    }

    {
        int buf[] = {0, 1, 2, 3, 4, 3, 4};
        ulib::span<int> ints(buf);

        int buf2[] = {3, 4};
        ulib::span<int> ints2(buf2);

        ASSERT_EQ(ints.rfind(3), 5);
        ASSERT_EQ(ints.rfind(20), ulib::npos);
        ASSERT_EQ(ints.rfind(ints2), 5);
    }
}

TEST(SpanTest, Contains)
{
    int buf[5] = {0, 1, 2, 3, 4};
    ulib::span<int> ints(buf);

    int buf2[3] = {0, 1, 2};
    ulib::span<int> ints2(buf2);

    int buf3[3] = {4, 1, 2};
    ulib::span<int> ints3(buf3);

    int buf4[3] = {1, 2, 3};
    ulib::span<int> ints4(buf4);

    ASSERT_TRUE(ints.contains(0));
    ASSERT_FALSE(ints.contains(-4));

    ASSERT_TRUE(ints.contains(ints2));
    ASSERT_TRUE(ints.contains(ints2));
    ASSERT_FALSE(ints.contains(ints3));
}

TEST(SpanTest, At)
{
    int buf[5] = {0, 1, 2, 3, 4};
    ulib::span<int> ints(buf);

    ASSERT_EQ(ints.at(0), 0);
    ASSERT_EQ(ints.at(1), 1);
    ASSERT_EQ(ints.at(2), 2);
    ASSERT_NE(ints.at(4), 2);

    ASSERT_THROW(ints.at(7), std::out_of_range);
}

TEST(SpanTest, ReverseIterator)
{
    int buf[5] = {0, 1, 2, 3, 4};
    ulib::span<int> ints(buf);

    {
        size_t vv = 4;
        for (auto it = ints.rbegin(); it != ints.rend(); it++)
        {
            ASSERT_EQ(*it, vv);
            vv--;
        }
    }

    {
        size_t vv = 4;
        for (auto &obj : ints.reverse())
        {
            ASSERT_EQ(obj, vv);
            vv--;
        }
    }
}

TEST(SpanTest, Assignment)
{
    int buf[5] = {0, 1, 2, 3, 4};
    ulib::span<int> ints(buf);
    ulib::span<int> ints2;

    ints2 = buf;
    
    ASSERT_TRUE(ints2 == buf);
    ASSERT_TRUE(ints2 == ints);

    ints2 = ints;
    ASSERT_TRUE(ints2 == buf);
    ASSERT_TRUE(ints2 == ints);
}

TEST(SpanTest, SubSpan)
{
    int buf[5] = {0, 1, 2, 3, 4};
    int buf2[3] = {1, 2, 3};

    ulib::span<int> ints(buf);
    auto ints2 = ints.subspan(1, 3);

    ASSERT_TRUE(ints2 == buf2);
}