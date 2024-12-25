#pragma once

#include "linearallocator.h"
#include <assert.h>
#include <cstddef>

namespace ulib
{
    template <class AllocatorT>
    class GrowLinearAllocator : public AllocatorT
    {
    public:

        struct Block
        {
            Block *next;
        };

        GrowLinearAllocator(typename AllocatorT::Params params = {})
            : AllocatorT(params)
        {
            mNextBlock = nullptr;
            mStep = 4096;
        }

        ~GrowLinearAllocator()
        {
            Block *block = mNextBlock;
            while (block)
            {
                Block *del = block;
                block = block->next;
                AllocatorT::Free(del);
            }
        }

        inline void *Alloc(size_t size)
        {
            if (void *ptr = mLinearAllocator.AttemptAlloc(size))
                return ptr;

            size_t allocSize = size > mStep ? size : mStep;
            void* newmem = AllocNewBlock(allocSize);
            mStep += allocSize;

            mLinearAllocator.Reset(newmem, allocSize);
            void *ptr = mLinearAllocator.AttemptAlloc(size);
            assert(ptr); // ???

            return ptr;
        }

        inline void Free(void *ptr)
        {
            // ...
        }

        inline void Clear()
        {
            if (mNextBlock)
            {
                Block *block = mNextBlock->next;

                while (block)
                {
                    Block *del = block;
                    // Block* next = block->next;

                    // block->next = nullptr;
                    block = block->next;

                    AllocatorT::Free(del);
                    
                }

                mNextBlock->next = nullptr;            
            }

            mLinearAllocator.Clear();
        }

    private:
        inline void* AllocNewBlock(size_t contentSize)
        {
            Block *block = (Block*)AllocatorT::Alloc(sizeof(Block) + contentSize);
            block->next = mNextBlock;
            mNextBlock = block;
            return block + 1;
        }

        LinearAllocator mLinearAllocator;
        size_t mStep;
        Block *mNextBlock;
    };
}