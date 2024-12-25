#pragma once

#include "baseresource.h"
#include <assert.h>

#include <ulib/types.h>

namespace ulib
{
    struct DebugPageFooter;
    struct DebugPageHeader
    {
        size_t pageSize;
        size_t hash;
        DebugPageFooter *footer;
        uint32 headerSize;
        uint32 signature;
    };

    struct DebugPageFooter
    {
        size_t hash;
    };

    template <class AllocatorT>
    class DebugResource : public BaseResource<AllocatorT>
    {
    public:
        inline DebugResource() { mUsage = 0; }
        inline DebugResource(typename AllocatorT::Params params) : BaseResource<AllocatorT>(params) { mUsage = 0; }
        inline DebugResource(const DebugResource<AllocatorT> &source) : BaseResource<AllocatorT>(source) { mUsage = 0; }

        inline DebugResource(DebugResource &&source) : BaseResource<AllocatorT>(std::move(source))
        {
            mUsage = source.mUsage;
            source.mUsage = 0;
        }

        inline ~DebugResource() { assert(mUsage == 0 && "Memory leak detected by ulib::DebugResource. Resource memory usage must be == 0 before destroying resource"); }

        inline DebugResource<AllocatorT> &operator=(DebugResource<AllocatorT> &&source)
        {
            BaseResource<AllocatorT> &rs = *this;
            rs = std::move(source);

            assert(mUsage == 0 && "Memory leak detected by ulib::DebugResource. Resource memory usage must be == 0 before moving resource");

            mUsage = source.mUsage;
            source.mUsage = 0;

            return *this;
        }

        static size_t HardRand()
        {
            if constexpr (sizeof(size_t) >= 8)
            {
                return size_t(uint64(rand()) + (uint64(rand()) << 16) + (uint64(rand()) << 32) + (uint64(rand()) << 48));
            }
            else if constexpr (sizeof(size_t) == 4)
            {
                return size_t(uint64(rand()) + (uint64(rand()) << 16));
            }
            else
            {
                return size_t(uint64(rand()));
            }
        }

        inline void *Alloc(size_t size)
        {
            size_t eq = size + sizeof(DebugPageHeader) + sizeof(DebugPageFooter);

            DebugPageHeader *header = (DebugPageHeader *)BaseResource<AllocatorT>::Alloc(eq);
            assert(header != nullptr && "nullptr returned in BaseResource<AllocatorT>::Alloc");

            DebugPageFooter *footer = (DebugPageFooter *)((char *)header + (size + sizeof(DebugPageHeader)));

            header->signature = 0xBEA91703;
            header->headerSize = uint(sizeof(*header));
            header->pageSize = eq;
            header->footer = footer;
            header->hash = HardRand();
            footer->hash = header->hash;

            mUsage += eq;
            return header + 1;
        }

        inline void Free(void *ptr)
        {
            if (!ptr)
                return;

            DebugPageHeader *header = (DebugPageHeader *)ptr - 1;
            DebugPageFooter *footer = (DebugPageFooter *)((char *)header + (header->pageSize - sizeof(DebugPageFooter)));

            assert(header->signature != 0xA123B123 &&
                   "Heap corruption detected by ulib::DebugResource. Detected multiple free page signature in page header.");
            assert(header->signature == 0xBEA91703 &&
                   "Heap corruption detected by ulib::DebugResource. Detected invalid page signature in page header.");
            assert(header->headerSize == sizeof(DebugPageHeader) &&
                   "Heap corruption detected by ulib::DebugResource. Detected invalid header size in page header");
            assert(header->footer == footer &&
                   "Heap corruption detected by ulib::DebugResource. Detected invalid page size or footer pointer in page header.");
            assert(header->hash == footer->hash &&
                   "Heap corruption detected by ulib::DebugResource. Detected difference between page header hash and page footer hash");
            assert(
                mUsage >= header->pageSize &&
                "Heap corruption detected by ulib::DebugResource. Current resource memory usage must be bigger or equal than size of page to free");

            mUsage -= header->pageSize;

            header->signature = 0xA123B123;
            header->footer = nullptr;
            header->headerSize = 0;
            header->pageSize = 0;
            header->hash = 0;
            footer->hash = size_t(-1);

            BaseResource<AllocatorT>::Free(header);
        }

    protected:
        size_t mUsage;
    };
} // namespace ulib