#include <array>
#include <gtest/gtest.h>
#include <numeric>
#include <ulib/containers/list.h>
#include <ulib/range.h>

TEST(ListTest, DefaultConstructor)
{
    ulib::list<int> list;

    ASSERT_TRUE(list.empty());
    ASSERT_TRUE(list.Size() == 0);
}

TEST(ListTest, SizeConstructor)
{
    ulib::List<int> list(40);

    ASSERT_FALSE(list.empty());
    ASSERT_TRUE(list.Size() == 40);
    ASSERT_TRUE(list.capacity() >= 40);
}

TEST(ListTest, CapacityConstructor)
{
    ulib::List<int> list(ulib::args::Capacity(40));

    ASSERT_TRUE(list.capacity() == 40);
    ASSERT_TRUE(list.available() == 40);
}

TEST(ListTest, InitializerListConstructor)
{
    ulib::List<int> list = std::initializer_list<int>{0, 1, 2, 3, 4};

    ASSERT_TRUE(list.capacity() >= 5);
    ASSERT_TRUE(list.size() == 5);

    int index = 0;
    for (auto v : list)
    {
        ASSERT_EQ(v, index);
        index++;
    }
}

TEST(ListTest, CopyConstructor)
{
    ulib::List<int> list0 = std::initializer_list<int>{0, 1, 2, 3, 4};
    ulib::List<int> list = list0;

    ASSERT_TRUE(list.capacity() >= 5);
    ASSERT_TRUE(list.size() == 5);
    ASSERT_TRUE(!list.empty());

    int index = 0;
    for (auto v : list)
    {
        ASSERT_EQ(v, index);
        index++;
    }
}

TEST(ListTest, ConstructFromSpan)
{
    int arr[] = {0, 1, 2, 3, 4};
    ulib::span<int> spn = arr;
    ulib::List<int> list = spn;

    ASSERT_TRUE(list.capacity() >= 5);
    ASSERT_TRUE(list.size() == 5);
    ASSERT_TRUE(!list.empty());

    int index = 0;
    for (auto v : list)
    {
        ASSERT_EQ(v, index);
        index++;
    }
}

TEST(ListTest, Resize)
{
    // equal resize
    {
        ulib::list<int> list = {0, 1, 2, 3, 4, 5, 6};
        list.resize(7);

        ASSERT_TRUE(list.capacity() >= 7);
        ASSERT_EQ(list.size(), 7);
        ASSERT_EQ(list.back(), 6);
    }

    // bigger resize
    {
        ulib::list<int> list = {0, 1, 2, 3, 4, 5, 6};
        list.resize(1000);

        ASSERT_TRUE(list.capacity() >= 1000);
        ASSERT_EQ(list.size(), 1000);
        ASSERT_EQ(list[4], 4);
    }

    // smaller resize
    {
        ulib::list<int> list = {0, 1, 2, 3, 4, 5, 6};
        list.resize(4);

        ASSERT_TRUE(list.capacity() >= 7);
        ASSERT_EQ(list.size(), 4);
        ASSERT_EQ(list.back(), 3);
    }

    // smaller then bigger resize
    {
        ulib::list<int> list = {0, 1, 2, 3, 4, 5, 6};
        list.resize(4);

        ASSERT_TRUE(list.capacity() >= 7);
        ASSERT_EQ(list.size(), 4);
        ASSERT_EQ(list.back(), 3);

        list.resize(1000);
        ASSERT_TRUE(list.capacity() >= 1000);
        ASSERT_EQ(list.size(), 1000);
        ASSERT_EQ(list[1], 1);
    }

    // combined resize
    {
        ulib::list<int> list = {0, 1, 2, 3, 4, 5, 6};

        list.resize(4);
        ASSERT_TRUE(list.capacity() >= 7);
        ASSERT_EQ(list.size(), 4);
        ASSERT_EQ(list.back(), 3);

        list.resize(1000);
        ASSERT_TRUE(list.capacity() >= 1000);
        ASSERT_EQ(list.size(), 1000);
        ASSERT_EQ(list[1], 1);

        list.resize(10);
        ASSERT_TRUE(list.capacity() >= 1000);
        ASSERT_EQ(list.size(), 10);
        ASSERT_EQ(list[2], 2);
    }
}

TEST(ListTest, NestedContainers)
{
    ASSERT_NO_FATAL_FAILURE({
        for (auto i : ulib::range(80))
        {
            ulib::List<ulib::List<ulib::List<int>>> list;
            list.resize(i);
        }
    });
}

TEST(ListTest, WithString)
{
    ulib::List<std::string> list = {"hello", "world"};

    ASSERT_EQ(list.size(), 2);
    ASSERT_EQ(list[0], "hello");
    ASSERT_EQ(list[1], "world");
}

TEST(ListTest, FrontAndBack)
{
    ulib::List<int> list = {0, 1, 2, 3, 4, 5};

    ASSERT_EQ(list.front(), 0);
    ASSERT_EQ(list.back(), 5);
}

TEST(ListTest, Compare)
{
    ulib::List<int> list = {0, 1, 2, 3, 4, 5};
    ulib::List<int> list0 = {0, 1, 2, 3, 4, 5};
    ulib::List<int> list1 = {3, 4, 5};

    ASSERT_TRUE(list.compare(list));
    ASSERT_TRUE(list == list);

    ASSERT_TRUE(list == list0);
    ASSERT_TRUE(list != list1);

    {
        int arr[] = {0, 1, 2, 3, 4, 5};
        int arr0[] = {0, 1, 2, 3, 4, 5};
        int arr1[] = {3, 4, 5};

        ASSERT_TRUE(list.compare(arr));
        ASSERT_TRUE(list == arr);

        ASSERT_TRUE(list == arr0);
        ASSERT_TRUE(list != arr1);
    }
}

TEST(ListTest, Inserting)
{
    {
        ulib::List<int> list = {0, 1, 2, 3, 4, 5};
        ulib::List<int> list0 = {0, 1, 20, 2, 3, 4, 5};
        ulib::List<int> list1 = {1, 2, 3};

        list.insert(list.at_index(2), 20);
        ASSERT_EQ(list, list0);

        list.insert(list.end(), 20);
        ASSERT_EQ(list.back(), 20);

        list.insert_back(40);
        ASSERT_EQ(list.back(), 40);

        list.insert_front(28);
        ASSERT_EQ(list.front(), 28);

        list.insert_front({1, 2, 3});
        ASSERT_EQ(list.subspan(0, 3), list1);
    }

    {
        ulib::List<int> list = {0, 1, 2, 3};
        ulib::List<int> list0 = {0, 1, 5, 6, 7, 2, 3};

        list.insert(list.at_index(2), {5, 6, 7});
        ASSERT_EQ(list, list0);
    }

    {
        ulib::List<int> list = {0, 1, 2, 3};
        ulib::List<int> list0 = {5, 6, 7, 0, 1, 2, 3};

        list.insert(list.begin(), {5, 6, 7});
        ASSERT_EQ(list, list0);
    }

    {
        ulib::List<int> list = {0, 1, 2, 3};
        ulib::List<int> list0 = {0, 1, 2, 3, 5, 6, 7};

        list.insert(list.end(), {5, 6, 7});
        ASSERT_EQ(list, list0);
    }
}

TEST(ListTest, Emplace)
{
    {
        ulib::List<int> list = {0, 1, 2, 3, 4, 5};
        ulib::List<int> list0 = {0, 1, 20, 2, 3, 4, 5};
        ulib::List<int> list1 = {0, 1, 20, 2, 3, 4, 5, 40};
        ulib::List<int> list2 = {60, 0, 1, 20, 2, 3, 4, 5, 40};

        list.emplace(list.at_index(2), 20);
        ASSERT_EQ(list, list0);

        auto &bb = list.emplace_back(40);
        ASSERT_EQ(list, list1);
        ASSERT_EQ(bb, 40);
        ASSERT_EQ(bb, list.back());

        auto &ff = list.emplace_front(60);
        ASSERT_EQ(list, list2);
        ASSERT_EQ(ff, 60);
        ASSERT_EQ(ff, list.front());
    }
}

TEST(ListTest, InsertingStrings)
{
    {
        ulib::List<std::string> list = {"one", "two", "three", "four"};
        ulib::List<std::string> list0 = {"one", "two", "five", "three", "four"};
        ulib::List<std::string> list1 = {"100", "200", "300"};

        list.insert(list.at_index(2), "five");
        ASSERT_EQ(list, list0);

        list.insert(list.end(), "six");
        ASSERT_EQ(list.back(), "six");

        list.insert_back("seven");
        ASSERT_EQ(list.back(), "seven");

        list.insert_front("eight");
        ASSERT_EQ(list.front(), "eight");

        list.insert_front({"100", "200", "300"});
        ASSERT_EQ(list.subspan(0, 3), list1);
    }

    {
        ulib::List<std::string> list = {"0", "1", "2", "3"};
        ulib::List<std::string> list0 = {"0", "1", "5", "6", "7", "2", "3"};

        list.insert(list.at_index(2), {"5", "6", "7"});
        ASSERT_EQ(list, list0);
    }

    {
        ulib::List<std::string> list = {"0", "1", "2", "3"};
        ulib::List<std::string> list0 = {"5", "6", "7", "0", "1", "2", "3"};

        list.insert(list.begin(), {"5", "6", "7"});
        ASSERT_EQ(list, list0);
    }

    {
        ulib::List<std::string> list = {"0", "1", "2", "3"};
        ulib::List<std::string> list0 = {"0", "1", "2", "3", "5", "6", "7"};

        list.insert(list.end(), {"5", "6", "7"});
        ASSERT_EQ(list, list0);
    }
}

TEST(ListTest, EmplaceStrings)
{
    {
        ulib::List<std::string> list = {"one", "two", "three", "four"};
        ulib::List<std::string> list0 = {"one", "two", "five", "three", "four"};
        ulib::List<std::string> list1 = {"one", "two", "five", "three", "four", "six"};
        ulib::List<std::string> list2 = {"seven", "one", "two", "five", "three", "four", "six"};

        list.emplace(list.at_index(2), "five");
        ASSERT_EQ(list, list0);

        auto &bb = list.emplace_back("six");
        ASSERT_EQ(list, list1);
        ASSERT_EQ(bb, "six");
        ASSERT_EQ(bb, list.back());

        auto &ff = list.emplace_front("seven");
        ASSERT_EQ(list, list2);
        ASSERT_EQ(ff, "seven");
        ASSERT_EQ(ff, list.front());
    }
}

TEST(ListTest, Append)
{
    {
        ulib::List<std::string> list = {"one", "two", "three"};
        ulib::List<std::string> list0 = {"one", "two", "three", "four", "five"};

        list += ulib::list<std::string>{"four", "five"};
        ASSERT_EQ(list, list0);

        list.pop_back(2);

        list.append({"four", "five"});
        ASSERT_EQ(list, list0);
    }
}

TEST(ListTest, Sum)
{
    {
        ulib::List<std::string> list = {"one", "two", "three"};
        ulib::List<std::string> list0 = {"one", "two", "three", "four", "five"};

        auto list1 = list + ulib::list<std::string>{"four", "five"};
        ASSERT_EQ(list1, list0);
    }
}

// TEST(ListTest, SelfAppend)
// {
//     {
//         ulib::List<std::string> list = {"one", "two", "three"};
//         ulib::List<std::string> list0 = {"one", "two", "three", "one", "two", "three"};

//         list += list;
//         ASSERT_EQ(list, list0);
//     }
// }

TEST(ListTest, SelfSum)
{
    {
        ulib::List<std::string> list = {"one", "two", "three"};
        ulib::List<std::string> list0 = {"one", "two", "three", "one", "two", "three"};

        list = list + list;
        ASSERT_EQ(list, list0);
    }
}

TEST(ListTest, SupportsAddingValues)
{
    ulib::List<int> list;

    ASSERT_NO_THROW(list.Add(3));
    ASSERT_NO_THROW(list.Add(4));
    ASSERT_NO_THROW(list.Add(5));

    EXPECT_EQ(list.Size(), 3);
}

TEST(ListTest, Resizes)
{
    ulib::List<int> list(10);
    std::iota(list.begin(), list.end(), 0);

    EXPECT_EQ(list.Size(), 10);

    list.Resize(5);
    EXPECT_EQ(list.Size(), 5);
    ASSERT_EQ(list.Capacity() >= list.Size(), true);
}

TEST(ListTest, Clears)
{
    ulib::List<int> list(10);
    std::iota(list.begin(), list.end(), 0);

    EXPECT_EQ(list.Size(), 10);

    ASSERT_NO_FATAL_FAILURE(list.Clear());
    EXPECT_EQ(list.Size(), 0);
}

TEST(ListTest, NullSizeInitialization)
{
    ulib::List<char> list;
    EXPECT_EQ(list.Size(), 0);
}

TEST(ListTest, FastErase)
{
    ulib::List<int> list(10);
    std::iota(list.begin(), list.end(), 0);

    list.FastErase(7);

    EXPECT_EQ(list.Size(), 9);
    EXPECT_EQ(list[7], 9);
}

TEST(ListTest, FastEraseFull)
{
    ulib::List<int> list(10);
    size_t prevSize = list.Size();

    while (list.Size())
    {
        list.FastErase(list.begin());
        EXPECT_NE(prevSize, list.Size());
        prevSize = list.Size();
    }
}

TEST(ListTest, EraseFull)
{
    ulib::List<int> list(10);
    std::iota(list.begin(), list.end(), 0);

    size_t prevSize = list.Size();
    while (list.Size())
    {
        list.Erase(0, 1);
        EXPECT_NE(prevSize, list.Size());
        prevSize = list.Size();
    }
}

TEST(ListTest, SequenceAfterErase)
{
    ulib::List<int> list(10);
    std::iota(list.begin(), list.end(), 0);

    ASSERT_EQ(list[4], 4);
    list.Erase(4, 1);

    ASSERT_EQ(list[4], 5);
    ASSERT_EQ(list.Size(), 9);
    ASSERT_EQ(list.Capacity() >= list.Size(), true);
}

TEST(ListTest, Enumerating)
{
    ulib::List<int> list(10);
    std::iota(list.begin(), list.end(), 0);

    ASSERT_EQ(list.Size(), 10);
    ASSERT_EQ(list.Capacity() >= list.Size(), true);

    size_t counter = 0;
    for (auto obj : list)
    {
        ASSERT_EQ(counter, obj);
        counter++;
    }

    ASSERT_EQ(counter, 10);
}

TEST(ListTest, Pushing)
{
    ulib::List<int> list;
    ASSERT_EQ(list.Size(), 0);

    for (size_t i = 0; i != 600; i++)
    {
        int value = rand();
        list.PushBack(value);

        ASSERT_EQ(list.Size() <= list.Capacity(), true);
        ASSERT_EQ(list.Size(), i + 1);
        ASSERT_EQ(list.Back(), value);
    }

    ASSERT_EQ(list.Size(), 600);

    for (size_t i = 600; i != 1200; i++)
    {
        int value = rand();
        list.push_back(value);

        ASSERT_EQ(list.size() <= list.capacity(), true);
        ASSERT_EQ(list.size(), i + 1);
        ASSERT_EQ(list.back(), value);
    }

    ASSERT_EQ(list.size(), 1200);
}

TEST(ListTest, Pop)
{
    ulib::List<int> list;
    ASSERT_EQ(list.Size(), 0);

    for (size_t i = 0; i != 600; i++)
    {
        int value = rand();
        list.PushBack(value);

        ASSERT_EQ(list.Size() <= list.Capacity(), true);
        ASSERT_EQ(list.Size(), i + 1);
        ASSERT_EQ(list.Back(), value);
    }

    ASSERT_EQ(list.Size(), 600);

    size_t prevSize = list.Size();
    while (list.Size())
    {
        list.Pop();
        ASSERT_EQ(list.Size(), prevSize - 1);
        prevSize = list.Size();
    }

    ASSERT_EQ(list.Size(), 0);
    ASSERT_EQ(list.Begin(), list.End());
}

TEST(ListTest, Indexing)
{
    ulib::List<int> list(600);
    std::iota(list.begin(), list.end(), 0);

    ASSERT_EQ(list.At(8), 8);
    ASSERT_EQ(list.At(17), 17);
    ASSERT_EQ(list.At(200), 200);

    ASSERT_EQ(list.at(8), 8);
    ASSERT_EQ(list.at(17), 17);
    ASSERT_EQ(list.at(200), 200);

    ASSERT_EQ(list[8], 8);
    ASSERT_EQ(list[17], 17);
    ASSERT_EQ(list[200], 200);

    for (size_t i = 0; i != 600; i++)
    {
        ASSERT_EQ(list.At(i), i);
        ASSERT_EQ(list.at(i), i);
        ASSERT_EQ(list[i], i);
    }

    for (size_t i = 0; i != 600; i++)
    {
        size_t rr = rand() % 600;

        ASSERT_EQ(list.At(rr), rr);
        ASSERT_EQ(list.At(rr), rr);
        ASSERT_EQ(list.At(rr), rr);
    }
}

TEST(ListTest, Moving)
{
    ulib::List<int> list(600);
    std::iota(list.begin(), list.end(), 0);

    auto list2 = std::move(list);
    ASSERT_EQ(list2.Size(), 600);
    ASSERT_EQ(list.Data(), nullptr);

    for (size_t i = 0; i != 600; i++)
    {
        ASSERT_EQ(list2.At(i), i);
        ASSERT_EQ(list2.at(i), i);
        ASSERT_EQ(list2[i], i);
    }
}

TEST(ListTest, SelfContains)
{
    using InList = ulib::List<int>;

    ulib::List<InList> list;
    for (size_t i = 0; i != 600; i++)
    {
        list.Add(InList{0, 1, 2, 3, 4, 5});
    }

    ASSERT_EQ(list.Size(), 600);

    for (size_t i = 0; i != 600; i++)
    {
        ASSERT_EQ(list[i].Size(), 6);
        ASSERT_EQ(list[i].Capacity() >= 6, true);

        size_t index = 0;
        for (auto it = list[i].begin(); it != list[i].end(); it++)
        {
            ASSERT_EQ(*it, index);
            index++;
        }
    }

    ASSERT_EQ(list.Size(), 600);
}

#include <ulib/string.h>

TEST(ListTest, ConstructsToSpan)
{
    const ulib::List<int> list;
    ulib::span<const int> spn = list;

    ASSERT_EQ(spn, list);
}

TEST(ListTest, ConstructsFromSpan)
{
    const ulib::List<int> list1;
    ulib::span<const int> spn = list1;

    ASSERT_EQ(spn, list1);

    ulib::List<int> list2 = spn;

    ASSERT_EQ(list1, list2);
}

// this test checks memory corruption in internal functions
TEST(ListTest, PushBackAfterCopy)
{
    for (size_t i = 0; i != 64; i++)
    {
        ulib::list<size_t> ints = {};
        ulib::list<size_t> ints2 = ints;

        for (size_t j = 0; j != 512; j++)
        {
            ints2.push_back(j);
            ASSERT_EQ(ints2.back(), j);
        }
    }
}

TEST(ListTest, InsertFrontAfterCopy)
{
    for (size_t i = 0; i != 64; i++)
    {
        ulib::list<size_t> ints = {};
        ulib::list<size_t> ints2 = ints;

        for (size_t j = 0; j != 512; j++)
        {
            ints2.insert_front(j);
            ASSERT_EQ(ints2.front(), j);
        }
    }
}

TEST(ListTest, InsertBackAfterCopy)
{
    for (size_t i = 0; i != 64; i++)
    {
        ulib::list<size_t> ints = {};
        ulib::list<size_t> ints2 = ints;

        for (size_t j = 0; j != 512; j++)
        {
            ints2.insert_back(j);
            ASSERT_EQ(ints2.back(), j);
        }
    }
}