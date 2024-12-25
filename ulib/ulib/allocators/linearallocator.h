#pragma once

#include <ulib/types.h>
#include <new>
#include <assert.h>
#include <cstddef>

namespace ulib
{
    class LinearAllocator
    {
    public:
        struct Params
        {
            Params(void *_begin, void *_end)
            {
                begin = _begin;
                end = _end;
            }

            Params(void *ptr, size_t size)
            {
                begin = ptr;
                end = rawptr_t(ptr) + size;
            }

            void *begin;
            void *end;
        };

        LinearAllocator()
        {
            mBegin = nullptr;
            mIt = nullptr;
            mEnd = nullptr;
        }

        LinearAllocator(Params params)
        {
            mBegin = rawptr_t(params.begin);
            mIt = rawptr_t(params.begin);
            mEnd = rawptr_t(params.end);

           assert(mEnd >= mBegin);
        }

        ~LinearAllocator()
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
            if (mIt + size <= mEnd)
            {
                assert(Left() >= size); // size_t overflow protection

                void *ptr = mIt;
                mIt += size;
                return ptr;
            }

            return nullptr;
        }

        inline void *Alloc(size_t size)
        {
            void *ptr = AttemptAlloc(size);
            if (!ptr)
                throw std::bad_alloc{};

            return ptr;
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