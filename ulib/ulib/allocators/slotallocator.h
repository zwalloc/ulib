#pragma once

#include "slotmanager.h"

#include <assert.h>

namespace ulib
{
    template <class T, class AllocatorT>
    class SlotAllocator : public AllocatorT
    {
    public:
        SlotAllocator(typename AllocatorT::Params params = {})
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