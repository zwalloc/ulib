#pragma once

#include <ulib/containers/list.h>

namespace ulib
{
    template <class T, class AllocatorT = DefaultAllocator>
    class FastQueue : public List<T, AllocatorT>
    {
    public:
        using SelfT = FastQueue<T, AllocatorT>;

        using value_type = T;
        using Iterator = RandomAccessIterator<value_type>;
        using ConstIterator = RandomAccessIterator<const value_type>;
        using ReverseIterator = ReverseRandomAccessIterator<value_type>;
        using ConstReverseIterator = ReverseRandomAccessIterator<const value_type>;
        using ReverseT = ReversedSpan<const value_type>;

        using ContainerTypeT = list_type_tag;
        using ContainerOwnershipT = store_ownership_tag;

        using pointer = value_type *;
        using const_pointer = const value_type *;
        using reference = value_type &;
        using const_reference = const value_type &;
        using iterator = Iterator;
        using const_iterator = ConstIterator;
        using reverse_iterator = ReverseIterator;
        using const_reverse_iterator = ConstReverseIterator;
        using size_type = size_t;
        using SpanT = Span<T>;
        using ListT = List<T, AllocatorT>;
        using AllocatorParams = typename AllocatorT::Params;

        FastQueue(AllocatorParams params = {}) : ListT(params), mIdx(0) {}
        FastQueue(SelfT &&other) : ListT(std::move(other)), mIdx(other.mIdx) {}

        FastQueue &operator=(SelfT &&other)
        {
            ListT &li = *this;
            li = std::move(other);

            mIdx = other.mIdx;
        }

        // inline iterator Begin() { return m.Begin(); }
        // inline iterator End() { return m.End(); }
        // inline const_iterator Begin() const { return m.Begin(); }
        // inline const_iterator End() const { return m.End(); }

        // inline reference Front() { return m.Front(); }
        // inline reference Back() { return m.Back(); }
        // inline const_reference Front() const { return m.Front(); }
        // inline const_reference Back() const { return m.Back(); }

        inline void Push(const_reference o) { ListT::PushBack(o); }
        inline void Push(T &&o) { ListT::PushBack(o); }
        inline void Pop() { ListT::PopBack(); }

        // inline bool Empty() const { return m.Empty(); }
        // inline size_type Size() const { return m.Size(); }
        // inline size_type SizeInBytes() const { return m.SizeInBytes(); }
        // inline size_type Capacity() const { return m.Capacity(); }
        // inline size_type CapacityInBytes() const { return m.CapacityInBytes(); }
        // inline size_type Available() const { return m.Available(); }
        // inline size_type AvailableInBytes() const { return m.AvailableInBytes(); }
        inline void Clear() { ListT::Clear(), mIdx = 0; }

        inline bool Peek(reference out)
        {
            size_t size = ListT::Size();
            if (!size)
                return false;

            if (mIdx == size)
            {
                ListT::Clear();
                return false;
            }

            out = std::move(ListT::At(mIdx));
            mIdx++;

            return true;
        }

        // inline iterator begin() { return Begin(); }
        // inline iterator end() { return End(); }
        // inline const_iterator begin() const { return Begin(); }
        // inline const_iterator end() const { return End(); }

        // inline reference front() { return Front(); }
        // inline reference back() { return Back(); }
        // inline const_reference front() const { return Front(); }
        // inline const_reference back() const { return Back(); }

        inline void push(const T &o) { Push(o); }
        inline void push(T &&o) { Push(o); }
        inline void pop() { Pop(); }

        // inline bool empty() const { return Empty(); }
        // inline size_type size() const { return Size(); }
        // inline size_type size_in_bytes() const { return SizeInBytes(); }
        // inline size_type capacity() const { return Capacity(); }
        // inline size_type capacity_in_bytes() const { return CapacityInBytes(); }
        // inline size_type available() const { return Available(); }
        // inline size_type available_in_bytes() const { return AvailableInBytes(); }
        // inline void clear() { Clear(); }

        inline bool peek(T &out) { return Peek(out); }

    private:
        size_t mIdx;
        // ulib::List<T, AllocatorT> m;
    };
} // namespace ulib