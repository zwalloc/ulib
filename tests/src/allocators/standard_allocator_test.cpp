#include <ulib/allocators/defaultallocator.h>
#include <ulib/allocators/growlinearallocator.h>
#include <ulib/allocators/standardallocator.h>
#include <ulib/allocators/staticallocator.h>

#include <gtest/gtest.h>
#include <thread>

#include <list>
#include <vector>

TEST(StandardAllocatorTest, FromDefaultAllocator)
{
    using Allocator = ulib::StandardAllocator<int, ulib::StaticAllocator<ulib::DefaultAllocator>>;

    {
        std::vector<int, Allocator> vec;
        vec.reserve(4000);

        ASSERT_TRUE(vec.capacity() >= 4000);
    }

    {
        std::list<int, Allocator> list;
        list.resize(4000);

        ASSERT_TRUE(list.size() == 4000);
    }
}

TEST(StandardAllocatorTest, FromGrowLinearAllocator)
{
    using Allocator =
        ulib::StandardAllocator<int, ulib::StaticAllocator<ulib::GrowLinearAllocator<ulib::DefaultAllocator>>>;

    {
        std::vector<int, Allocator> vec;
        vec.reserve(4000);

        ASSERT_TRUE(vec.capacity() >= 4000);
    }

    {
        std::list<int, Allocator> list;
        list.resize(4000);

        ASSERT_TRUE(list.size() == 4000);
    }
}