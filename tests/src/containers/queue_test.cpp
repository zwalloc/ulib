#include <gtest/gtest.h>

#include <ulib/containers/queue.h>
#include <ulib/string.h>

TEST(QueueTest, PushAndPopAndFrontAndBack)
{
    ulib::FastQueue<ulib::string> queue;
    queue.push("hello");
    queue.push("world");

    ASSERT_EQ(queue.front(), "hello");
    ASSERT_EQ(queue.back(), "world");

    queue.pop();

    ASSERT_EQ(queue.front(), "hello");
    ASSERT_EQ(queue.back(), "hello");
}

TEST(QueueTest, Sizing)
{
    ulib::FastQueue<ulib::string> queue;
    ASSERT_EQ(queue.size(), 0);
    ASSERT_EQ(queue.size_in_bytes(), 0);
    ASSERT_EQ(queue.empty(), true);

    queue.push("hello");
    ASSERT_EQ(queue.size(), 1);
    ASSERT_EQ(queue.size_in_bytes(), sizeof(ulib::string));
    ASSERT_EQ(queue.empty(), false);

    queue.push("world");
    ASSERT_EQ(queue.size(), 2);

    queue.pop();
    ASSERT_EQ(queue.size(), 1);
}

TEST(QueueTest, Peek)
{
    ulib::FastQueue<ulib::string> queue;
    queue.push("hello");
    queue.push("world");

    int idx = 0;
    ulib::string text;
    while (queue.peek(text))
    {
        if (idx == 0)
        {
            ASSERT_EQ(text, "hello");
        }

        if (idx == 1)
        {
            ASSERT_EQ(text, "world");
        }

        idx++;
    }
}

TEST(QueueTest, Foreach)
{
    ulib::FastQueue<ulib::string> queue;
    queue.push("hello");
    queue.push("world");
    int idx = 0;
    for (auto& text : queue)
    {
        if (idx == 0)
        {
            ASSERT_EQ(text, "hello");
        }

        if (idx == 1)
        {
            ASSERT_EQ(text, "world");
        }

        idx++;
    }
}