#include <ulib/allocators/fastmemallocator.h>
#include <gtest/gtest.h>

TEST(FastMemAllocatorTest, CanAllocAndFree)
{
    constexpr auto kAllocSize = 16;

    ulib::FastMemAllocator<ulib::DefaultAllocator> allocator;

    void* ptr = allocator.Alloc(kAllocSize);    
    ASSERT_NE(ptr, nullptr);
    ASSERT_NO_FATAL_FAILURE(allocator.Free(ptr));
}
