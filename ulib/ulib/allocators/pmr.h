#pragma once

#include <cstddef>

namespace ulib
{
    struct IAllocator
    {
        static IAllocator *Default();
        virtual ~IAllocator() {}

        virtual void *Alloc(size_t size) = 0;
        virtual void Free(void *page) = 0;
    };

    namespace pmr
    {
        class AllocatorReference
        {
        public:
            struct Params
            {
                Params(IAllocator *_pAllocator)
                    : pAllocator(_pAllocator)
                {
                }

                Params(IAllocator &_pAllocator)
                    : pAllocator(&_pAllocator)
                {
                }

                IAllocator *pAllocator;
            };

            AllocatorReference(Params params)
                : mAllocator(params.pAllocator)
            {
            }

            inline void *Alloc(size_t size) { return mAllocator->Alloc(size); }
            inline void Free(void *page) { mAllocator->Free(page); }

        private:
            IAllocator *mAllocator;
        };

        template <class AllocatorT>
        class Allocator : public IAllocator, public AllocatorT
        {
        public:
            Allocator(typename AllocatorT::Params params = {})
                : AllocatorT(params)
            {
            }

            virtual void *Alloc(size_t size) { return AllocatorT::Alloc(size); }
            virtual void Free(void *page) { AllocatorT::Free(page); }
        };
    }
}

inline void *operator new(size_t size, ulib::IAllocator &al) { return al.Alloc(size); }
inline void operator delete(void *ptr, ulib::IAllocator &al) { return al.Free(ptr); }