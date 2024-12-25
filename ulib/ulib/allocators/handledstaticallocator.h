#pragma once

#include <cstddef>
#include <ulib/typetraits/handledsingleton.h>
#include "allocatortag.h"

namespace ulib
{
    template <class AllocatorT, class TagT = AllocatorTag<DefaultTag>>
    class HandledStaticAllocator
    {
    public:
        using Current = HandledSingleton<AllocatorT, TagT>;
        using AllocatorParams = typename AllocatorT::Params;

        struct Params
        {
        };

        inline static AllocatorT &Instance() { return Current::Instance(); }
        inline static AllocatorT &Construct(AllocatorParams params = {}) { return Current::Construct(params); }

        HandledStaticAllocator() {}
        HandledStaticAllocator(Params params) {}
        HandledStaticAllocator(HandledStaticAllocator<AllocatorT, TagT> &&source) {}
        ~HandledStaticAllocator() {}

        HandledStaticAllocator<AllocatorT, TagT> &operator=(HandledStaticAllocator<AllocatorT, TagT> &&source) { return *this; }

        inline static void *Alloc(size_t size) { return Instance().Alloc(size); }
        inline static void Free(void *ptr) { Instance().Free(ptr); }
    };

}