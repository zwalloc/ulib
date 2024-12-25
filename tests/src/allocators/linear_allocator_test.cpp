#include <ulib/allocators/linearallocator.h>
#include <gtest/gtest.h>

TEST(LinearAllocatorTest, CanAllocAndFree)
{
    constexpr auto kAllocSize = 16;

    char buffer[256] = {0};

    ulib::LinearAllocator allocator{{buffer, buffer + sizeof buffer}};

    void* ptr = allocator.Alloc(kAllocSize);    
    ASSERT_NE(ptr, nullptr);
    ASSERT_NO_FATAL_FAILURE(allocator.Free(ptr));

    ASSERT_NE(allocator.Alloc(kAllocSize), nullptr);
    ASSERT_NO_FATAL_FAILURE(allocator.Clear());
}
