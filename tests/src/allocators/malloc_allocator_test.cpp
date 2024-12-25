#include <ulib/allocators/mallocallocator.h>
#include <gtest/gtest.h>

TEST(MallocAllocatorTest, CanAllocAndFree)
{
    constexpr auto kAllocSize = 16;

    ulib::MallocAllocator allocator;

    void* ptr = allocator.Alloc(kAllocSize);    
    ASSERT_NE(ptr, nullptr);
    ASSERT_NO_FATAL_FAILURE(allocator.Free(ptr));
}
