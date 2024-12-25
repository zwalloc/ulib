#pragma once

#include <ulib/types.h>
#include <new>
#include <assert.h>
#include <cstddef>

namespace ulib
{
    class UnsafeLinearAllocator
    {
    public:
        struct Params
        {
            inline Params()
            {
                begin = nullptr;
                end = nullptr;
            }

            inline Params(void *_begin, void *_end)
            {
                begin = _begin;
                end = _end;
            }

            inline Params(void *ptr, size_t size)
            {
                begin = ptr;
                end = rawptr_t(ptr) + size;
            }

            void *begin;
            void *end;
        };

        inline UnsafeLinearAllocator()
        {
            mBegin = nullptr;
            mIt = nullptr;
            mEnd = nullptr;
        }

        inline UnsafeLinearAllocator(Params params)
        {
            mBegin = rawptr_t(params.begin);
            mIt = rawptr_t(params.begin);
            mEnd = rawptr_t(params.end);

            assert(mEnd >= mBegin);
        }

        inline ~UnsafeLinearAllocator()
        {
        }

        inline void Clear()
        {
            mIt = mBegin;
        }

        inline void Reset(void *begin, void *end)
        {
            mBegin = rawptr_t(begin);
            mIt = rawptr_t(begin);
            mEnd = rawptr_t(end);

            assert(mEnd >= mBegin);
        }

        inline void Reset(void *ptr, size_t size)
        {
            Reset(ptr, rawptr_t(ptr) + size);
        }

        inline void *AttemptAlloc(size_t size)
        {
            assert(Left() >= size);

            void *ptr = mIt;
            mIt += size;
            return ptr;
        }

        inline void *Alloc(size_t size)
        {
            return AttemptAlloc(size);
        }

        inline void Free(void *ptr)
        {
        }

        inline size_t Left()
        {
            return mEnd - mIt;
        }

    private:
        rawptr_t mBegin;
        rawptr_t mIt;
        rawptr_t mEnd;
    };
}