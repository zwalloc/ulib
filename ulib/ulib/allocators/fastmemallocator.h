#pragma once

#include <ulib/allocators/defaultallocator.h>
#include <ulib/resources/resource.h>


#include "pagemanager.h"
#include "rtslotmanager.h"

#include <cstddef>

namespace ulib
{

    /*
        class MemoryReadStream
    {
    public:
        inline MemoryReadStream(void *data, size_t size)
        {
            mIt = rawptr_t(data);
            mEnd = mIt + size;

            assert(mIt <= mEnd);
        }

        inline void *Read(size_t size)
        {
            assert(mIt < mEnd);
            assert(size <= Left());

            void *ptr = mIt;
            mIt += size;

            memset(ptr, 0x11, size);
            return ptr;
        }

        template <class T>
        inline T *Read()
        {
            assert(mIt < mEnd);
            assert(sizeof(T) <= Left());

            void *ptr = mIt;
            mIt += sizeof(T);

            memset(ptr, 0x11, sizeof(T));
            return (T *)ptr;
        }

        inline size_t Left()
        {
            assert(mEnd >= mIt);

            return mEnd - mIt;
        }

    private:
        rawptr_t mIt;
        rawptr_t mEnd;
    };
    */

    template <class T>
    constexpr T max(T v1, T v2)
    {
        return v1 > v2 ? v1 : v2;
    }

    template <class AllocatorT, size_t PoolStep = 64, size_t PoolCount = 64, size_t MinimumNewPageDataLength = 0x40>
    class FastMemAllocator : public Resource<AllocatorT>
    {
    public:
        inline static constexpr size_t kPoolAllocMax = PoolStep * PoolCount;
        inline static constexpr size_t kPoolArraySize = PoolCount;
        inline static constexpr size_t kPagerIndex = 0x1000;
        inline static constexpr size_t kMaxPoolIndex = PoolCount - 1;

        using PagerT = BasicPageManager<pages::DefaultPage, MinimumNewPageDataLength>;

        static inline constexpr size_t GetPoolIndex(size_t allocSize) { return ((allocSize - 1) / PoolStep); }

        static inline constexpr size_t GetPoolSlotSize(size_t index) { return (index + 1) * PoolStep; }

        struct Block
        {
            Block *next;
        };

        struct Params
        {
            inline Params() {}

            inline Params(typename AllocatorT::Params params) { allocatorParams = params; }

            typename AllocatorT::Params allocatorParams;
        };

        FastMemAllocator(Params params = {}) : Resource<AllocatorT>(params.allocatorParams)
        {
            mNextBlock = nullptr;

            // SlotManager *data = (SlotManager *)AllocNewBlock(sizeof(SlotManager) * PoolCount);

            for (size_t i = 0; i != PoolCount; i++)
            {
                mSlotSteps[i] = GetPoolStartupAllocSize(i);
                // mSlotManagers[i] = &data[i];
                new (&mSlotManagers[i]) RtSlotManager(GetPoolSlotSize(i));
            }

            mPagerStep = GetPoolStartupAllocSize(kMaxPoolIndex) * 32;
        }

        ~FastMemAllocator()
        {
            // for (SlotManager& obj : mSlotManagers)
            // obj->~SlotManager();

            Block *block = mNextBlock;

            while (block)
            {
                Block *del = block;
                block = block->next;

                Resource<AllocatorT>::Free(del);
            }
        }

        static constexpr size_t GetPoolStartupSlotsCount(size_t idx) { return 0; }

        static constexpr size_t GetPoolStartupAllocSize(size_t idx)
        {
            size_t reverseSlotSize = GetPoolSlotSize(kMaxPoolIndex - idx);
            size_t slotSize = GetPoolSlotSize(idx);

            if (reverseSlotSize > slotSize)
            {
                size_t slotCount = reverseSlotSize / slotSize;
                return slotCount * slotSize;
            }
            else
            {
                return slotSize;
            }
            // auto req = max(GetPoolSlotSize(kMaxPoolIndex - idx), GetPoolSlotSize(idx));
        }

        static constexpr size_t GetPoolsStartupAllocSize()
        {
            size_t size = 0;
            for (size_t i = 0; i != PoolCount; i++)
                size += GetPoolStartupAllocSize(i);

            return size;
        }

        static constexpr size_t GetStartupRequiredSize()
        {
            size_t reqSize = GetPoolsStartupAllocSize();

            reqSize += sizeof(RtSlotManager) * PoolCount;
            reqSize += 1024 * 1024;

            return reqSize;
        }

        inline void *Alloc(size_t size)
        {
            size_t allocSize = size + sizeof(size_t);
            size_t *page = (size_t *)AllocInternal(allocSize);

            // printf("0x%p\n", page);
            return page + 1;

            return nullptr;
        }

        inline void Free(void *ptr)
        {
            size_t *index = ((size_t *)ptr - 1);
            if (*index == kPagerIndex)
            {
                mPager.Free(index);
            }
            else
            {
                assert(*index < 64);

                mSlotManagers[*index].Free(index);
            }
        }

    private:
        inline void *AllocViaPools(size_t index)
        {
            RtSlotManager &slotmgr = mSlotManagers[index];
            if (void *ptr = slotmgr.Alloc())
                return ptr;

            size_t allocSize = mSlotSteps[index] * 4;
            void *newmem = AllocNewBlock(allocSize);

            slotmgr.AddFreePage(newmem, allocSize);
            mSlotSteps[index] = allocSize;

            return slotmgr.Alloc();
        }

        inline void *AllocViaPager(size_t realSize)
        {
            if (void *ptr = mPager.Alloc(realSize))
                return ptr;

            size_t allocSize = (mPagerStep > realSize ? mPagerStep : realSize) + PagerT::FreePageOverhead();
            void *newmem = AllocNewBlock(allocSize);

            mPagerStep += allocSize;

            mPager.AddFreePage(newmem, allocSize);
            return mPager.Alloc(realSize);
        }

        inline void *AllocInternal(size_t realSize)
        {
            size_t index = GetPoolIndex(realSize);
            if (index < PoolCount)
            {
                void *ptr = AllocViaPools(index);
                *(size_t *)ptr = index;
                return ptr;
            }
            else
            {
                void *ptr = AllocViaPager(realSize);
                *(size_t *)ptr = kPagerIndex;
                return ptr;
            }
        }

        inline void *AllocNewBlock(size_t size)
        {
            Block *block = (Block *)Resource<AllocatorT>::Alloc(size + sizeof(Block));

            block->next = mNextBlock;
            mNextBlock = block;

            return block + 1;
        }

        RtSlotManager mSlotManagers[PoolCount];
        size_t mSlotSteps[PoolCount];
        PagerT mPager;
        size_t mPagerStep;
        Block *mNextBlock;
    };
} // namespace ulib

template <class AllocatorT, size_t x, size_t y, size_t z>
inline void *operator new(size_t size, ulib::FastMemAllocator<AllocatorT, x, y, z> &al)
{
    return al.Alloc(size);
}

template <class AllocatorT, size_t x, size_t y, size_t z>
inline void operator delete(void *ptr, ulib::FastMemAllocator<AllocatorT, x, y, z> &al)
{
    return al.Free(ptr);
}
