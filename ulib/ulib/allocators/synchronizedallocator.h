#pragma once

#include <mutex>
#include <cstddef>

namespace ulib
{
    template <class AllocatorT, class MutexT = std::mutex>
    class SynchronizedAllocator : public AllocatorT
    {
    public:
        SynchronizedAllocator(typename AllocatorT::Params params = {})
            : AllocatorT(params)
        {
        }

        ~SynchronizedAllocator()
        {
        }

        inline void *Alloc(size_t size)
        {
            std::lock_guard l(mMtx);
            return AllocatorT::Alloc(size);
        }

        inline void Free(void *page)
        {
            std::lock_guard l(mMtx);
            AllocatorT::Free(page);
        }

    private:
        MutexT mMtx;
    };
}