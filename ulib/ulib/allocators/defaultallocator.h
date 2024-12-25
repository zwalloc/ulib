#pragma once

#include <assert.h>
#include <cstddef>

#ifndef __has_builtin         // Optional of course.
  #define __has_builtin(x) 0  // Compatibility with non-clang compilers.
#endif

namespace ulib
{
    class DefaultAllocator
    {
    public:
        struct Params
        {
        };

        DefaultAllocator(Params params = {}) {}

        inline void *Alloc(size_t size) const
        {
#if __has_builtin(__builtin_operator_new)
            return __builtin_operator_new(size);
#else
            return new char[size];
#endif
        }
        inline void Free(void *page) const
        {
            assert(page);

#if __has_builtin(__builtin_operator_delete)
            __builtin_operator_delete(page);
#else
            delete[] (char*)page;
#endif
        }

    private:
    };
} // namespace ulib

inline void *operator new(size_t size, ulib::DefaultAllocator &al) { return al.Alloc(size); }
inline void operator delete(void *ptr, ulib::DefaultAllocator &al) { return al.Free(ptr); }