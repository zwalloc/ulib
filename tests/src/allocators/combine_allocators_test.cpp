#include <ulib/allocators/staticallocator.h>
#include <ulib/allocators/synchronizedallocator.h>
#include <ulib/allocators/mallocallocator.h>

#include <gtest/gtest.h>

TEST(CombineAllocatorsTest, MallocAllocator)
{
    constexpr auto kAllocSize = 16;

    using Allocator = ulib::StaticAllocator<ulib::SynchronizedAllocator<ulib::MallocAllocator>>;

    void* ptr = Allocator::Alloc(kAllocSize);    
    ASSERT_NE(ptr, nullptr);
    ASSERT_NO_FATAL_FAILURE(Allocator::Free(ptr));
}