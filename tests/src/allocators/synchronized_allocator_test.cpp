#include <ulib/allocators/defaultallocator.h>
#include <ulib/allocators/growlinearallocator.h>
#include <ulib/allocators/linearallocator.h>
#include <ulib/allocators/mallocallocator.h>
#include <ulib/allocators/staticallocator.h>
#include <ulib/allocators/synchronizedallocator.h>
#include <ulib/allocators/fastmemallocator.h>
#include <ulib/list.h>


#include <gtest/gtest.h>
#include <thread>

TEST(SynchronizedAllocatorTest, SynchronizedGrowLinearAllocator)
{
    using Allocator = ulib::SynchronizedAllocator<ulib::GrowLinearAllocator<ulib::DefaultAllocator>>;

    Allocator al;
    auto loop =
        [&]() {
            ulib::List<void *> list;
            for (size_t i = 0; i != 30000; i++)
            {
                list.push_back(al.Alloc(40));
            }

            for (void *ptr : list)
            {
                al.Free(ptr);
            }
        };

    std::thread th = std::thread(loop);
    loop();
    th.join();
}

TEST(SynchronizedAllocatorTest, SynchronizedFastMemAllocator)
{
    using Allocator = ulib::SynchronizedAllocator<ulib::FastMemAllocator<ulib::DefaultAllocator>>;

    Allocator al;
    auto loop =
        [&]() {
            ulib::List<void *> list;
            for (size_t i = 0; i != 30000; i++)
            {
                list.push_back(al.Alloc(40));
            }

            for (void *ptr : list)
            {
                al.Free(ptr);
            }
        };

    std::thread th = std::thread(loop);
    loop();
    th.join();
}