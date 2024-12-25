#include <ulib/allocators/growlinearallocator.h>
#include <ulib/allocators/defaultallocator.h>
#include <gtest/gtest.h>

TEST(GrowLinearAllocatorTest, CanAllocAndFree)
{
    constexpr auto kAllocSize = 16;

    ulib::GrowLinearAllocator<ulib::DefaultAllocator> allocator;

    void* ptr = allocator.Alloc(kAllocSize);    
    ASSERT_NE(ptr, nullptr);
    ASSERT_NO_FATAL_FAILURE(allocator.Free(ptr));

    ASSERT_NE(allocator.Alloc(kAllocSize), nullptr);
    ASSERT_NO_FATAL_FAILURE(allocator.Clear());
}
