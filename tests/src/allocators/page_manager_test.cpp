#include <ulib/allocators/pagemanager.h>
#include <gtest/gtest.h>

TEST(PageManagerTest, CanAllocAndFree)
{
    // constexpr size_t kAllocSize = 16;
    constexpr size_t kBufferSize = ulib::PageManager::FreePageOverhead() + 1024;

    ulib::PageManager pager;
    char buffer[kBufferSize];
    pager.AddFreePage(buffer, kBufferSize);

    void *ptr = pager.Alloc(100);
    ASSERT_NE(ptr, nullptr);
    ASSERT_NO_FATAL_FAILURE(pager.Free(ptr));
}

TEST(PageManagerTest, FullFreeSpaceAllocation)
{
    constexpr size_t kFreeSize = 1024;
    constexpr size_t kBufferSize = ulib::PageManager::FreePageOverhead() + kFreeSize;

    ulib::PageManager pager;
    char buffer[kBufferSize];

    pager.AddFreePage(buffer, kBufferSize);
    void *ptr = pager.Alloc(kFreeSize);
    ASSERT_NE(ptr, nullptr);

    void *ptr2 = pager.Alloc(1);
    ASSERT_EQ(ptr2, nullptr);

    ASSERT_NO_FATAL_FAILURE(pager.Free(ptr));
}

TEST(PageManagerTest, CombinedAllocationOfFullSpace)
{
    constexpr size_t kFreeSize = 1024;
    constexpr size_t kBufferSize = ulib::PageManager::BlockOverhead() + kFreeSize;
    constexpr size_t kAllocSize = (kFreeSize / 2) - ulib::PageManager::PageOverhead();

    char buffer[kBufferSize];
    ulib::PageManager pager;
    pager.AddFreePage(buffer, kBufferSize);

    void *ptr0 = pager.Alloc(kAllocSize);
    void *ptr1 = pager.Alloc(kAllocSize);

    ASSERT_NE(ptr0, nullptr);
    ASSERT_NE(ptr1, nullptr);

    // check free space
    void *ptrn = pager.Alloc(1);
    ASSERT_EQ(ptrn, nullptr);

    ASSERT_NO_FATAL_FAILURE(pager.Free(ptr0));
    ASSERT_NO_FATAL_FAILURE(pager.Free(ptr1));

    // full space allocation
    void *ptrf = pager.Alloc(kFreeSize - ulib::PageManager::PageOverhead());
    ASSERT_NE(ptrf, nullptr);

    // check free space
    ptrn = pager.Alloc(1);
    ASSERT_EQ(ptrn, nullptr);

    ASSERT_NO_FATAL_FAILURE(pager.Free(ptrf));
}

TEST(PageManagerTest, PageMergingOnFree)
{
    using Pager = ulib::BasicPageManager<ulib::pages::DefaultPage, 0>;

    constexpr size_t kAllocCount = 10;
    constexpr size_t kAllocSize = 1;
    constexpr size_t kBufferSize = kAllocCount * (kAllocSize + Pager::PageOverhead()) + Pager::BlockOverhead();

    char buffer[kBufferSize];
    Pager pager;
    pager.AddFreePage(buffer, kBufferSize);

    ASSERT_EQ(pager.CalcFreePages(), 1);

    void* pages[kAllocCount] = { 0 };
    for(size_t i = 0; i != kAllocCount; i++)
    {
        void* ptr = pager.Alloc(kAllocSize);
        ASSERT_NE(ptr, nullptr);

        pages[i] = ptr;
    }

    ASSERT_EQ(pager.CalcFreePages(), 0);

    for(size_t i = 0; i != kAllocCount; i++)
    {
        ASSERT_NO_FATAL_FAILURE(pager.Free(pages[i]));
    }

    ASSERT_EQ(pager.CalcFreePages(), 1);
}

TEST(PageManagerTest, CrashOnNullptrFree)
{
    ulib::PageManager pager;
    ASSERT_DEATH(pager.Free(nullptr), "");
}