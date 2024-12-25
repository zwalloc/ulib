
#pragma once

/*
#include "rtslotmanager.h"

#include <assert.h>

namespace ulib
{
    template <class AllocatorT>
    class RtSlotAllocator : public AllocatorT
    {
    public:

        struct Params
        {
            Params()
            {

            }

            size_t step;
            size_t slotSize;
            AllocatorT::Params allocatorParams;
        };

        SlotAllocator(AllocatorT::Params params = {})
            : AllocatorT(params)
        {
            mStep = 64;
        }

        SlotAllocator(const SlotAllocator& al) = delete;
        SlotAllocator(SlotAllocator&&) = delete;

        ~SlotAllocator()
        {
        }

        inline void *Alloc(size_t size)
        {
            assert(mSlotManager.kSlotSize >= size);

            if (void *ptr = mSlotManager.Alloc())
                return ptr;

            size_t allocSize = mStep;
            void *newmem = AllocatorT::Alloc(allocSize);
            assert(newmem);

            mSlotManager.AddFreePage(newmem, allocSize);
            mStep = allocSize + allocSize;

            void *ptr = mSlotManager.Alloc();
            assert(ptr); // ???

            return ptr;
        }

        inline void Free(void *ptr)
        {
            assert(ptr);

            mSlotManager.Free(ptr);
        }

        inline T *Alloc()
        {
            return (T *)Alloc(sizeof(T));
        }

    private:
        SlotManager<T> mSlotManager;
        size_t mStep;
    };
}
*/


