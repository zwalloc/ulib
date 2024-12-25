#include <ulib/allocators/defaultallocator.h>
#include <gtest/gtest.h>

TEST(DefaultAllocatorTest, CanAllocAndFree)
{
    constexpr auto kAllocSize = 16;

    ulib::DefaultAllocator allocator;

    void* ptr = allocator.Alloc(kAllocSize);    
    ASSERT_NE(ptr, nullptr);
    ASSERT_NO_FATAL_FAILURE(allocator.Free(ptr));
}
