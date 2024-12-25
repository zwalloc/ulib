#pragma once

#include <new>
#include <cstddef>

namespace ulib
{
    template <class AllocatorT, int iIndex, class Allocator2T = AllocatorT>
    class StaticPointerAllocator
    {
    public:
        using Params = typename AllocatorT::Params;

        static AllocatorT &Instance(typename AllocatorT::Params *params = nullptr)
        {
            static AllocatorT *ptr = nullptr;
            if (ptr)
                return *ptr;

            ptr = (AllocatorT *)Allocator2T::Alloc(sizeof(*ptr));

            if (params)
            {
                new (ptr) AllocatorT(*params);
            }
            else
            {
                new (ptr) AllocatorT;
            }

            return *ptr;
        }

        StaticPointerAllocator()
        {
            Instance();
        }

        StaticPointerAllocator(typename AllocatorT::Params params)
        {
            Instance(&params);
        }

        ~StaticPointerAllocator()
        {
        }

        static void *Alloc(size_t size)
        {
            return Instance().Alloc(size);
        }

        static void Free(void *ptr)
        {
            Instance().Free(ptr);
        }
    };
}