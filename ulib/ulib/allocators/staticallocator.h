#pragma once

#include <ulib/utility/singleton.h>
#include <cstddef>

#include "allocatortag.h"

namespace ulib
{
    struct DefaultAllocatorTag {};
    
    template <class AllocatorT, class TagT = DefaultAllocatorTag>
    class StaticAllocator
    {
    public:
        using Current = Singleton<AllocatorT, TagT>;
        using AllocatorParams = typename AllocatorT::Params;

        struct Params
        {};

        inline static AllocatorT &Instance(AllocatorParams params = {}) { return Current::Instance(params); }

        StaticAllocator() {}
        StaticAllocator(Params params) {}
        StaticAllocator(StaticAllocator<AllocatorT, TagT> &&source) {}
        StaticAllocator(const StaticAllocator<AllocatorT, TagT> &source) {}
        ~StaticAllocator() {}

        StaticAllocator<AllocatorT, TagT> &operator=(StaticAllocator<AllocatorT, TagT> &&source) { return *this; }

        inline static void *Alloc(size_t size) { return Instance().Alloc(size); }
        inline static void Free(void *ptr) { Instance().Free(ptr); }
    };

}