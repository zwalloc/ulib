#include <ulib/allocators/staticallocator.h>
#include <ulib/allocators/defaultallocator.h>

#include <gtest/gtest.h>

TEST(StaticAllocatorTest, CanAllocAndFree)
{
    constexpr auto kAllocSize = 16;

    using Allocator = ulib::StaticAllocator<ulib::DefaultAllocator>;

    void* ptr = Allocator::Alloc(kAllocSize);    
    ASSERT_NE(ptr, nullptr);
    ASSERT_NO_FATAL_FAILURE(Allocator::Free(ptr));
}