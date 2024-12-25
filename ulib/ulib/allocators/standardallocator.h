#pragma once

#include <assert.h>
#include <cstddef>

namespace ulib
{
    template <class T, class AllocatorT>
    class StandardAllocator : public AllocatorT
    {
    public:
        using value_type = T;

        template <class RebindT>
        struct rebind
        {
            using other = StandardAllocator<RebindT, AllocatorT>;
        };

        StandardAllocator(typename AllocatorT::Params params = {})
            : AllocatorT(params)
        {
        }

        template <class U>
        StandardAllocator(const StandardAllocator<U, AllocatorT> &other)
            : AllocatorT(typename AllocatorT::Params{})
        {
            /*
            if (!std::is_same<T, U>::value)
                throw std::exception(typeid(T).name());
            */           
        }

        StandardAllocator(const StandardAllocator<T, AllocatorT> &right)
            : AllocatorT(right)
        {
                      
        }

        inline void deallocate(T *_Ptr, size_t _Count)
        {
            assert(_Ptr);
            assert(_Count);

            AllocatorT::Free(_Ptr);
        }

        inline T *allocate(size_t _Count)
        {
            assert(_Count);

            return (T *)AllocatorT::Alloc(_Count * sizeof(T));
        }
    };
}