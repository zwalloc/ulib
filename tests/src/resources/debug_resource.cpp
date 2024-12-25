#ifdef _DEBUG

#include <gtest/gtest.h>

#include <ulib/allocators/defaultallocator.h>
#include <ulib/resources/debugresource.h>

TEST(DebugResourceTest, DetectMemoryLeak)
{
    using ResourceT = ulib::DebugResource<ulib::DefaultAllocator>;
    ResourceT *res = new ResourceT;

    char *data = (char *)res->Alloc(10);
    ASSERT_DEATH({ delete res; }, "Memory leak");
}

TEST(DebugResourceTest, DetectHeaderModificationOnFree)
{
    using ResourceT = ulib::DebugResource<ulib::DefaultAllocator>;
    ResourceT *res = new ResourceT;

    ASSERT_DEATH(
        {
            for (size_t i = 0; i != 400; i++)
            {
                char *data = (char *)res->Alloc(10);
                data[-1] = 0x0;

                res->Free(data);
            }
        },
        "Heap corruption");
}

TEST(DebugResourceTest, DetectFooterModificationOnFree)
{
    using ResourceT = ulib::DebugResource<ulib::DefaultAllocator>;
    ResourceT *res = new ResourceT;

    ASSERT_DEATH(
        {
            for (size_t i = 0; i != 400; i++)
            {
                char *data = (char *)res->Alloc(10);
                data[10] = 0x0;

                res->Free(data);
            }
        },
        "Heap corruption");
}

#endif
