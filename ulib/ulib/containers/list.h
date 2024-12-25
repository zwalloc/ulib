#pragma once

#include <ulib/allocators/defaultallocator.h>
#include <ulib/containers/args.h>
#include <ulib/containers/iterators/randomaccessiterator.h>
#include <ulib/containers/span.h>
#include <ulib/containers/tags.h>
#include <ulib/resources/resource.h>
#include <ulib/types.h>

#include <initializer_list>
#include <new> // overloaded new / delete

#include <assert.h>
#include <string.h> // memcpy

#include <functional>
#include <ulib/typetraits/rawmove.h>
#include <ulib/typetraits/typeordefault.h>
#include <utility>

#include <stdexcept>

#include "groupby.h"

namespace ulib
{
    enum class ExpandMemoryPolicy
    {
        Flex = 0,
        Strict = 1,
        Default = Flex
    };

    template <class T, class AllocatorTy = DefaultAllocator>
    class List;

    // template <class ContT, class LambdaT, class ValueT = typename ContT::value_type, class GroupT = std::invoke_result_t<LambdaT, ValueT>, class
    // AllocatorT = constainer_choose_ulib_allocator_t<ContT>> List<std::pair<GroupT, List<ValueT>>> group_by(const ContT &cont, LambdaT pred);

    template <class T, class AllocatorTy>
    class List : public Resource<AllocatorTy>
    {
    public:
        ULIB_RAW_MOVEABLE();

        using SelfT = List<T, AllocatorTy>;

        // standard fields
        using value_type = T;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        using reference = value_type &;
        using const_reference = const value_type &;
        using iterator = RandomAccessIterator<value_type>;
        using const_iterator = RandomAccessIterator<const value_type>;
        using reverse_iterator = ReverseRandomAccessIterator<value_type>;
        using const_reverse_iterator = ReverseRandomAccessIterator<const value_type>;
        using size_type = size_t;

        // ulib fields
        using ContainerTypeT = list_type_tag;
        using ContainerOwnershipT = store_ownership_tag;

        // ulib optional fields
        using AllocatorT = AllocatorTy;
        using ReverseT = ReversedSpan<const value_type>;
        using SpanT = Span<T>;
        using ViewT = SpanView<T>;

        // detail fields
        using BufferT = List<uchar, AllocatorT>;
        using ResourceT = Resource<AllocatorT>;
        using AllocatorParams = typename AllocatorT::Params;
        using InitializerListT = std::initializer_list<T>;
        using SplitViewT = SplitView<ViewT>;

        constexpr static size_t C_STEP = 16;
        constexpr static bool IsTrivial() { return std::is_trivially_copyable_v<T>; }
        constexpr static bool IsRawMovable() { return is_explicit_raw_moveable_flag_v<T, std::true_type> || IsTrivial(); }

        inline List(AllocatorParams al = {}) : ResourceT(al) { SetupViaCapacityB(C_STEP * sizeof(T)); }
        template <class IterT>
        inline List(IterT first, IterT last, AllocatorParams al = {}) : ResourceT(al)
        {
            SetupWithCopyConstructWithIterators(first, last);
        }

        inline List(const_pointer ptr, size_type count, AllocatorParams al = {}) : ResourceT(al) { SetupWithCopyConstruct(ptr, count); }

        template <class K, std::enable_if_t<is_container_from_range_constructible_v<SelfT, K> && has_container_data_method_v<K>, bool> = true>
        inline List(K &&cont, AllocatorParams al = {}) : ResourceT(al)
        {
            ViewT rn = cont;
            SetupWithCopyConstructB(rn.Data(), rn.SizeInBytes());
        }

        template <class K, std::enable_if_t<!(is_container_from_range_constructible_v<SelfT, K> && has_container_data_method_v<K>) &&
                                                has_container_begin_method_v<K> && std::is_constructible_v<value_type, typename K::value_type>,
                                            bool> = true>
        inline List(K &&cont, AllocatorParams al = {}) : ResourceT(al)
        {
            SetupWithCopyConstructWithIterators(cont.begin(), cont.end());
        }

        inline explicit List(size_type size, AllocatorParams al = {}) : ResourceT(al) { SetupViaSizeAndConstruct(size); }
        inline explicit List(args::Capacity capacity, AllocatorParams al = {}) : ResourceT(al) { SetupViaCapacity(capacity.cc); }
        inline List(InitializerListT init, AllocatorParams al = {}) : ResourceT(al) { SetupWithCopyConstruct(init.begin(), init.end()); }
        inline List(const SelfT &other) : ResourceT(other) { SetupWithCopyConstruct(other.mBegin, other.mLast); }
        inline List(SelfT &&source) : ResourceT(std::move(source))
        {
            mBegin = source.mBegin;
            mLast = source.mLast;
            mEnd = source.mEnd;

            source.mBegin = nullptr;
        }

        inline ~List()
        {
            if (mBeginB)
            {
                for (T *ptr = mBegin; ptr != mLast; ptr++)
                    ptr->~T();

                ResourceT::Free(mBeginB);
            }
        }

        inline iterator Begin() { return mBegin; }
        inline iterator End() { return mLast; }
        inline const_iterator Begin() const { return mBegin; }
        inline const_iterator End() const { return mLast; }
        inline reverse_iterator ReverseBegin() { return mLast - 1; }
        inline reverse_iterator ReverseEnd() { return mBegin - 1; }
        inline const_reverse_iterator ReverseBegin() const { return mLast - 1; }
        inline const_reverse_iterator ReverseEnd() const { return mBegin - 1; }
        inline ReverseT Reverse() const { return ReverseT{ReverseBegin(), ReverseEnd()}; }
        inline T *Data() { return mBegin; }
        inline const T *Data() const { return mBegin; }
        inline size_type Size() const { return mLast - mBegin; }
        inline size_type Length() const { return Size(); }
        inline size_type SizeInBytes() const { return mLastB - mBeginB; }
        inline size_type Capacity() const { return mEnd - mBegin; }
        inline size_type CapacityInBytes() const { return mEndB - mBeginB; }
        inline size_type Available() const { return mEnd - mLast; }
        inline size_type AvailableInBytes() const { return mEndB - mLastB; }
        inline bool Empty() const { return mLast == mBegin; }

        inline void AssignS(ViewT right)
        {
            size_t allocSize = right.SizeInBytes();
            size_t requiredSize = allocSize;

            for (T *ptr = mBegin; ptr != mLast; ptr++)
                ptr->~T();

            if (CapacityInBytes() < requiredSize)
            {
                ResourceT::Free(mBeginB);
                mEndB = (mBeginB = (uchar *)ResourceT::Alloc(allocSize)) + allocSize;
            }

            mLastB = mBeginB + requiredSize;
            CopyConstructElementsB(mBegin, right.Data(), requiredSize);
        }

        template <class K, enable_if_range_compatible_t<SelfT, K>>
        inline void Assign(const K &right)
        {
            AssignS(right);
        }
        inline void Assign(const SelfT &right) { AssignS(right); }
        inline void Assign(SelfT &&source)
        {
            // assert(mBegin && "Attempt to move via assign to moved List");
            assert(source.mBegin && "Attempt to move-assign moved List");

            if (mBegin)
            {
                for (T *ptr = mBegin; ptr != mLast; ptr++)
                    ptr->~T();

                ResourceT::Free(mBeginB);
            }

            Resource<AllocatorT> &rs = *this;
            rs = std::move(source);

            mBegin = source.mBegin;
            mLast = source.mLast;
            mEnd = source.mEnd;

            source.mBegin = nullptr;
        }

        inline void Resize(size_t newSize)
        {
            auto point = mBegin + newSize;
            if (point <= mLast)
            {
                if constexpr (!IsTrivial())
                {
                    for (auto it = point; it != mLast; it++)
                        it->~T();
                }

                mLast = point;

                return;
            }
            else
            {
                if (point > mEnd)
                {
                    ConcreteReallocateMemory(SizeInBytes(), newSize * sizeof(T));
                    // ReallocateMemory(SizeInBytes(), newSize * sizeof(T));
                    point = mBegin + newSize;
                }

                for (auto it = mLast; it != point; it++)
                    new (it) T();

                mLast = point;
            }
        }

        inline void Resize(size_t newSize, const_reference value)
        {
            auto point = mBegin + newSize;
            if (point <= mLast)
            {
                if constexpr (!IsTrivial())
                {
                    for (auto it = point; it != mLast; it++)
                        it->~T();
                }

                mLast = point;

                return;
            }
            else
            {
                if (point > mEnd)
                {
                    ConcreteReallocateMemory(SizeInBytes(), newSize * sizeof(T));
                    // ReallocateMemory(SizeInBytes(), newSize * sizeof(T));
                    point = mBegin + newSize;
                }

                for (auto it = mLast; it != point; it++)
                    new (it) T(value);

                mLast = point;
            }
        }

        inline iterator Insert(const_iterator pos, const_reference v)
        {
            size_t idx = GetIndex(pos);
            ReallocIfEnd();
            pos = mBegin + idx;

            auto it = Unbound(pos).Raw();

            {
                size_t count = 1;
                auto rbeg = mLast - 1;
                auto idest = rbeg + count;
                auto isrc = rbeg;
                auto dest_1 = it + count - 1;

                for (; idest != dest_1; idest--, isrc--)
                    new (idest) T(std::move(*isrc));
            }

            // memmove(it + 1, it, (mLast - it) * sizeof(T));
            ++mLast;

            new (it) T(v);
            return it;
        }

        inline iterator Insert(const_iterator pos, T &&v)
        {
            size_t idx = GetIndex(pos);
            ReallocIfEnd();
            pos = mBegin + idx;

            auto it = Unbound(pos).Raw();

            {
                size_t count = 1;
                auto rbeg = mLast - 1;
                auto idest = rbeg + count;
                auto isrc = rbeg;
                auto dest_1 = it + count - 1;

                for (; idest != dest_1; idest--, isrc--)
                    new (idest) T(std::move(*isrc));
            }

            // memmove(it + 1, it, (mLast - it) * sizeof(T));
            ++mLast;

            new (it) T(std::move(v));
            return it;
        }

        inline iterator Insert(const_iterator pos, size_type count, const_reference value)
        {
            size_t idx = GetIndex(pos);
            RequestFreeSpace(count);
            pos = mBegin + idx;

            auto it = Unbound(pos).Raw();
            {
                auto rbeg = mLast - 1;
                auto idest = rbeg + count;
                auto isrc = rbeg;
                auto dest_1 = it + count - 1;

                for (; idest != dest_1; idest--, isrc--)
                    new (idest) T(std::move(*isrc));
            }
            // memmove(it + count, it, (mLast - it) * sizeof(T));
            mLast += count;

            CopyConstructElements(pos, pos + count, value);
            return it;
        }

        inline iterator Insert(const_iterator pos, ViewT right) { return Insert(pos, right.Begin(), right.End()); }
        inline iterator Insert(const_iterator pos, InitializerListT right) { return Insert(pos, right.begin(), right.end()); }

        template <class InputIt>
        inline iterator Insert(const_iterator pos, InputIt first, InputIt last)
        {
            size_t count = last - first;

            size_t idx = GetIndex(pos);
            RequestFreeSpace(count);
            pos = mBegin + idx;

            auto it = Unbound(pos).Raw();

            {
                auto rbeg = mLast - 1;
                auto idest = rbeg + count;
                auto isrc = rbeg;
                auto dest_1 = it + count - 1;

                for (; idest != dest_1; idest--, isrc--)
                    new (idest) T(std::move(*isrc));
            }

            // memmove(it + count, it, (mLast - it) * sizeof(T));
            mLast += count;

            CopyConstructElements(&(*first), &(*first) + count, Unbound(pos).Raw());
            return it;
        }

        inline iterator InsertBack(const_reference v) { return Insert(End(), v); }
        inline iterator InsertBack(T &&v) { return Insert(End(), std::move(v)); }
        inline iterator InsertBack(size_type count, const_reference value) { return Insert(End(), count, value); }
        inline iterator InsertBack(ViewT right) { return Insert(End(), right); }
        inline iterator InsertBack(const_iterator pos, InitializerListT right) { return Insert(End(), right.begin(), right.end()); }
        template <class InputIt>
        inline iterator InsertBack(InputIt first, InputIt last)
        {
            return Insert(End(), first, last);
        }

        inline iterator InsertFront(const_reference v) { return Insert(Begin(), v); }
        inline iterator InsertFront(T &&v) { return Insert(Begin(), std::move(v)); }
        inline iterator InsertFront(size_type count, const_reference value) { return Insert(Begin(), count, value); }
        inline iterator InsertFront(ViewT right) { return Insert(Begin(), right); }
        inline iterator InsertFront(const_iterator pos, InitializerListT right) { return Insert(Begin(), right.begin(), right.end()); }
        template <class InputIt>
        inline iterator InsertFront(InputIt first, InputIt last)
        {
            return Insert(Begin(), first, last);
        }
        template <class... Args>
        inline iterator Emplace(const_iterator pos, Args &&...args)
        {
            size_t count = 1;

            size_t idx = GetIndex(pos);
            RequestFreeSpace(count);
            pos = mBegin + idx;

            auto it = Unbound(pos).Raw();

            // printf("\n[Emplace]\nsizeof(T): %d\nindex: %d\nit: %p\nmBegin: %p\nmLast: %p\nmEnd: %p\n"
            //        "size: %d\nsize_in_bytes: %d\ncapacity: %d\ncapacity_in_bytes: %d\n"
            //        "available: %d\navailable_in_bytes: %d\n",
            //        int(sizeof(T)), int(idx), it, mBegin, mLast, mEnd, int(size()), int(size_in_bytes()), int(capacity()), int(capacity_in_bytes()),
            //        int(available()), int(available_in_bytes()));

            if constexpr (IsRawMovable())
            {
                memmove(it + 1, it, (mLast - it) * sizeof(T));
            }
            else
            {
                size_t moveCount = (mLast - it);

                auto dest = it + 1;
                auto src = it;

                auto idest = (dest + moveCount) - 1;
                auto isrc = (src + moveCount) - 1;

                for (; idest != dest - 1; idest--, isrc--)
                {
                    new (idest) T(std::move(*isrc));
                }
            }

            new ((void *)it) T(std::forward<Args>(args)...);
            mLast += 1;

            return it;
        }
        template <class... Args>
        inline reference EmplaceBack(Args &&...args)
        {
            return *Emplace(End(), std::forward<Args>(args)...);
        }
        template <class... Args>
        inline reference EmplaceFront(Args &&...args)
        {
            return *Emplace(Begin(), std::forward<Args>(args)...);
        }

        inline SelfT &Append(ViewT right) { return InsertBack(right), *this; }

        inline T &Front()
        {
            assert(Size() && "Attempt get out of range element in List<T>::Front");
            return *mBegin;
        }
        inline const T &Front() const
        {
            assert(Size() && "Attempt get out of range element in List<T>::Front");
            return *mBegin;
        }
        inline T &Back()
        {
            assert(Size() && "Attempt get out of range element in List<T>::Back");
            return *(mLast - 1);
        }
        inline const T &Back() const
        {
            assert(Size() && "Attempt get out of range element in List<T>::Back");
            return *(mLast - 1);
        }

        inline void PushBack(const T &o)
        {
            ReallocIfEnd();
            new (mLast) T(o);
            mLast++;
        }

        inline void PushBack(T &&o)
        {
            ReallocIfEnd();
            new (mLast) T(std::move(o));
            mLast++;
        }

        inline void Add(const T &o) { PushBack(o); }
        inline void Add(T &&o) { PushBack(std::move(o)); }

        inline T &At(size_t i)
        {
            if (i >= Size())
                throw std::out_of_range{"Attempt get out of range element in List<T>::At"};

            return mBegin[i];
        }
        inline const T &At(size_t i) const
        {
            if (i >= Size())
                throw std::out_of_range{"Attempt get out of range element in List<T>::At"};
            return mBegin[i];
        }

        inline iterator Erase(const_iterator it)
        {
            assert(it.ptr >= mBegin && "Attempt erase out of range element in List<T>::Erase");
            assert(it.ptr < mLast && "Attempt erase out of range element in List<T>::Erase");

            it.ptr->~T();
            auto from = it + 1;
            ::memcpy(pointer(it.raw()), from.raw(), mLastB - (uchar *)from.raw());
            mLast--;
            return pointer(it.raw());
        }

        // inline iterator Erase(size_type i) { return Erase(mBegin + i); }
        inline iterator Erase(const_iterator first, const_iterator last)
        {
            for (auto it = first; it != last; it++)
                it.ptr->~T();

            auto from = last;
            ::memcpy(pointer(first.raw()), last.raw(), mLastB - (uchar *)last.raw());

            mLast -= last - first;
            return pointer(first.raw());
        }

        inline SelfT &Erase(size_type index = 0, size_type count = npos)
        {
            // size_t removeCount = std::min(count, size() - index);
            auto start = mBegin + index;
            auto finish = start + count;

            if (start != finish)
                Erase(start, finish);

            return *this;
        }

        inline iterator FastErase(const_iterator it)
        {
            assert(it.ptr >= mBegin && "Attempt erase out of range element in List<T>::FastErase");
            assert(it.ptr < mLast && "Attempt erase out of range element in List<T>::FastErase");

            T *pBack = mLast - 1;
            it.ptr->~T();

            if (it.ptr != pBack)
            {
                // c++ fan version
                // new (it.ptr) T(std::move(*pBack));

                // right version
                memcpy(pointer(it.ptr), pBack, sizeof(T));
            }

            mLast--;
            return pointer(it.raw());
        }

        inline iterator FastErase(size_type i) { return FastErase(mBegin + i); }

        inline bool Remove(const_reference elem)
        {
            for (auto it = mBegin; it != mLast; it++)
                if (*it == elem)
                    return Erase(it), true;
            return false;
        }

        inline bool FastRemove(const_reference elem)
        {
            for (auto it = mBegin; it != mLast; it++)
                if (*it == elem)
                    return FastErase(it), true;
            return false;
        }

        inline void Clear()
        {
            for (T *ptr = mBegin; ptr != mLast; ptr++)
                ptr->~T();
            mLast = mBegin;
        }

        inline void PopBack()
        {
            assert(Size() && "Attempt pop element in empty list in List<T>::Pop");

            mLast--;
            mLast->~T();
        }

        inline void PopBack(size_type count)
        {
            assert(Size() >= count && "Attempt pop element in empty list in List<T>::Pop");

            for (auto i = 0; i != count; i++)
            {
                mLast--;
                mLast->~T();
            }
        }

        inline void Pop() { PopBack(); }

        inline void Reserve(size_t s)
        {
            size_type curr = CapacityInBytes(), required = s * sizeof(T);
            if (curr >= required)
                return;

            ConcreteReallocateMemory(curr, required);
        }

        inline SpanT ToSpan() const { return SpanT{mBegin, mLast}; }
        inline ViewT ToView() const { return ViewT{mBegin, mLast}; }
        inline bool Compare(ViewT right) const { return ToSpan().Compare(right); }
        inline size_type Find(const_reference v, size_type pos = 0) const { return ToSpan().Find(v, pos); }
        inline size_type Find(ViewT v, size_type pos = 0) const { return ToSpan().Find(v, pos); }
        inline size_type ReverseFind(const_reference v, size_type pos = 0) const { return ToSpan().ReverseFind(v, pos); }
        inline size_type ReverseFind(ViewT v, size_type pos = 0) const { return ToSpan().ReverseFind(v, pos); }
        inline bool StartsWith(const_reference v) const { return ToSpan().StartsWith(v); }
        inline bool EndsWith(const_reference v) const { return ToSpan().EndsWith(v); }
        inline bool Contains(const_reference v) const { return ToSpan().Contains(v); }
        inline bool StartsWith(ViewT v) const { return ToSpan().StartsWith(v); }
        inline bool EndsWith(ViewT v) const { return ToSpan().EndsWith(v); }
        inline bool Contains(ViewT v) const { return ToSpan().Contains(v); }
        inline SpanT SubSpan(size_type pos, size_type n = npos) const { return ToSpan().SubSpan(pos, n); }

        inline size_type GetIndex(const_iterator it) const { return size_type(it.ptr - mBegin); }
        inline iterator GetIterator(size_type i) const { return mBegin + i; }

        inline SplitViewT Split(ViewT sep) const { return SplitViewT{*this, sep}; }
        inline BufferView Raw() const { return BufferView{mBeginB, mLastB}; }

        // operators

        inline SelfT &operator=(ViewT right) { return Assign(right), *this; }
        inline SelfT &operator=(const SelfT &right) { return Assign(right), *this; }
        inline SelfT &operator=(SelfT &&source) { return Assign(std::move(source)), *this; }

        // inline bool operator==(const SelfT &right) const { return Compare(right); }
        inline bool operator==(ViewT right) const { return Compare(right); }

        // inline bool operator!=(const SelfT &right) const { return !Compare(right); }
        inline bool operator!=(ViewT right) const { return !Compare(right); }

        inline T &operator[](size_t i) { return mBegin[i]; }
        inline const T &operator[](size_t i) const { return mBegin[i]; }

        inline SelfT operator+(ViewT right) const { return AdditionImpl(right); }
        inline SelfT &operator+=(ViewT right) { return Append(right); }
        // container aliases

        inline void assign(ViewT right) { return Assign(right); }
        inline void assign(SelfT &&source) { return Assign(std::move(source)); }
        inline void resize(size_t newSize) { return Resize(newSize); }
        inline void resize(size_t newSize, const_reference value) { return Resize(newSize, value); }

        inline iterator insert(const_iterator pos, const_reference v) { return Insert(pos, v); }
        inline iterator insert(const_iterator pos, T &&v) { return Insert(pos, std::move(v)); }
        inline iterator insert(const_iterator pos, size_type count, const_reference value) { return Insert(pos, count, value); }
        inline iterator insert(const_iterator pos, SpanT right) { return Insert(pos, right); }
        inline iterator insert(const_iterator pos, InitializerListT right) { return Insert(pos, right); }
        template <class InputIt>
        inline iterator insert(const_iterator pos, InputIt first, InputIt last)
        {
            return Insert(pos, first, last);
        }

        inline iterator insert_back(const_reference v) { return InsertBack(v); }
        inline iterator insert_back(T &&v) { return InsertBack(std::move(v)); }
        inline iterator insert_back(size_type count, const_reference value) { return InsertBack(count, value); }
        inline iterator insert_back(SpanT right) { return Insert(right); }
        inline iterator insert_back(InitializerListT right) { return InsertBack(right); }
        template <class InputIt>
        inline iterator insert_back(InputIt first, InputIt last)
        {
            return Insert(first, last);
        }

        inline iterator insert_front(const_reference v) { return InsertFront(v); }
        inline iterator insert_front(T &&v) { return InsertFront(std::move(v)); }
        inline iterator insert_front(size_type count, const_reference value) { return InsertFront(count, value); }
        inline iterator insert_front(SpanT right) { return InsertFront(right); }
        inline iterator insert_front(InitializerListT right) { return InsertFront(right); }
        template <class InputIt>
        inline iterator insert_front(InputIt first, InputIt last)
        {
            return Insert(first, last);
        }

        template <class... Args>
        inline iterator emplace(const_iterator pos, Args &&...args)
        {
            return Emplace(pos, std::forward<Args>(args)...);
        }
        template <class... Args>
        inline reference emplace_back(Args &&...args)
        {
            return EmplaceBack(std::forward<Args>(args)...);
        }
        template <class... Args>
        inline reference emplace_front(Args &&...args)
        {
            return EmplaceFront(std::forward<Args>(args)...);
        }
        inline SelfT &append(ViewT right) { return Append(right); }
        inline SelfT &append(InitializerListT right) { return Append(right); }

        inline iterator erase(const_iterator it) { return Erase(it); }
        // inline iterator erase(size_type i) { return Erase(i); }
        inline iterator erase(const_iterator first, const_iterator last) { return Erase(first, last); }
        inline SelfT &erase(size_type index = 0, size_type count = npos) { return Erase(index, count); }

        inline iterator ferase(const_iterator it) { return FastErase(it); }
        inline iterator ferase(size_type i) { return FastErase(i); }
        inline bool remove(const_reference elem) { return Remove(elem); }
        inline bool fremove(const_reference elem) { return Remove(elem); }

        inline size_type capacity() const { return Capacity(); }
        inline size_type capacity_in_bytes() const { return CapacityInBytes(); }
        inline size_type available() const { return Available(); }
        inline size_type available_in_bytes() const { return AvailableInBytes(); }
        inline void push_back(const T &o) { PushBack(o); }
        inline void push_back(T &&o) { PushBack(std::move(o)); }
        inline void clear() { return Clear(); };
        inline void pop_back() { PopBack(); }
        inline void pop_back(size_type count) { PopBack(count); }
        inline void pop() { Pop(); }

        // general aliases

        inline iterator begin() { return mBegin; }
        inline iterator end() { return mLast; }
        inline pointer data() { return Data(); }
        inline reverse_iterator rbegin() { return mLast - 1; }
        inline reverse_iterator rend() { return mBegin - 1; }
        inline reference at(size_type idx) { return At(idx); }
        inline reference front() { return Front(); }
        inline reference back() { return Back(); }

        // view aliases

        inline const_pointer data() const { return Data(); }
        inline const_iterator begin() const { return mBegin; }
        inline const_iterator end() const { return mLast; }
        inline const_reverse_iterator rbegin() const { return mLast - 1; }
        inline const_reverse_iterator rend() const { return mBegin - 1; }
        inline ReverseT reverse() const { return Reverse(); }
        inline bool empty() const { return Empty(); }
        inline size_type size() const { return Size(); }
        inline size_type size_in_bytes() const { return SizeInBytes(); }
        inline size_type length() const { return Length(); }
        inline const_reference at(size_type idx) const { return At(idx); }
        inline const_reference front() const { return Front(); }
        inline const_reference back() const { return Back(); }
        inline SpanT to_span() const { return ToSpan(); }
        inline ViewT to_view() const { return ToView(); }
        inline bool compare(SpanT right) const { return Compare(right); }
        inline size_type find(const_reference v, size_type pos = 0) const { return Find(v, pos); }
        inline size_type find(ViewT v, size_type pos = 0) const { return Find(v, pos); }
        inline size_type rfind(const_reference v, size_type pos = 0) const { return ReverseFind(v, pos); }
        inline size_type rfind(ViewT v, size_type pos = 0) const { return ReverseFind(v, pos); }
        inline bool starts_with(const_reference v) const { return StartsWith(v); }
        inline bool ends_with(const_reference v) const { return EndsWith(v); }
        inline bool contains(const_reference v) const { return Contains(v); }
        inline bool starts_with(ViewT v) const { return StartsWith(v); }
        inline bool ends_with(ViewT v) const { return EndsWith(v); }
        inline bool contains(ViewT v) const { return Contains(v); }
        inline SelfT subspan(size_type pos, size_type n = npos) const { return SubSpan(pos, n); }
        inline size_type iter_index(const_iterator it) const { return GetIndex(it); }
        inline const_iterator at_index(size_type i) const { return GetIterator(i); }
        inline SplitViewT split(InitializerListT sep) const { return SplitViewT{*this, sep}; }
        inline SplitViewT split(ViewT sep) const { return SplitViewT{*this, sep}; }
        inline BufferView raw() const { return Raw(); }

        /*

        template <class NewPred, class... NewArgs>
        auto map(NewPred &&pred, NewArgs &&...args) const
        {
            using result_value_t = std::invoke_result_t<NewPred, value_type, NewArgs...>;

            static_assert(!std::is_void_v<result_value_t>, "ulib::List<T>::map() does not support methods returning void for consistency."
                                                           "To invoke a method on all items without any return values, use ulib::List<T>::transform");

            using result_list_t = List<result_value_t, AllocatorT>;

            result_list_t result{args::Capacity(size())};
            for (auto it = mBegin; it != mLast; it++)
                result.push_back(pred(*it, args...));

            return MapView<SelfT, NewPred, NewArgs...>{*this, std::forward<NewPred>(pred), std::forward<NewArgs>(args)...};
        }
         */

        // Map Operations

        template <class Pred, class... Args>
        auto map(Pred &&pred, Args &&...args) const
        {
            return to_view().map(std::forward<Pred>(pred), std::forward<Args>(args)...);
        }

        template <class Pred, class... Args>
        auto map(Pred &&pred, Args &&...args)
        {
            return to_span().map(std::forward<Pred>(pred), std::forward<Args>(args)...);
        }

        // Filter Operations

        template <class Pred, class... Args>
        auto filter(Pred &&pred, Args &&...args) const
        {
            return to_view().filter(std::forward<Pred>(pred), std::forward<Args>(args)...);
        }

        template <class Pred, class... Args>
        auto filter(Pred &&pred, Args &&...args)
        {
            return to_span().filter(std::forward<Pred>(pred), std::forward<Args>(args)...);
        }

        // Transform Operations

        template <class... FuncArgs, class RetVal = void, class VT = value_type>
        void transform(RetVal (VT::*fn)(FuncArgs &&...), FuncArgs &&...args)
        {
            for (auto it = mBegin; it != mLast; it++)
                (it->*fn)(std::forward<FuncArgs>(args)...);
        }

        template <class... FuncArgs, class RetVal = void, class VT = value_type>
        void transform(RetVal (VT::*fn)(FuncArgs &&...) const, FuncArgs &&...args) const
        {
            for (const T *it = mBegin; it != mLast; it++)
                (it->*fn)(std::forward<FuncArgs>(args)...);
        }

        template <class F>
        void transform(F &&fn) const
        {
            for (auto it = mBegin; it != mLast; it++)
                fn(*it);
        }

        value_type reduce(std::function<value_type(reference, reference)> fn)
        {
            if (empty())
                throw std::logic_error{"attempt to reduce an empty list"};

            value_type result = *mBegin;
            for (auto it = mBegin + 1; it != mLast; it++)
            {
                result = fn(result, *it);
            }

            return result;
        }

        value_type reduce(std::function<value_type(reference, reference)> fn, value_type initialValue)
        {
            value_type result = initialValue;
            for (auto it = mBegin; it != mLast; it++)
            {
                result = fn(result, *it);
            }

            return result;
        }

        inline SelfT replace(ViewT from, ViewT to)
        {
            assert(!from.empty());
            // assert(!to.empty());

            assert(mLast >= mBegin);

            size_t fromLen = from.size();
            SelfT result;
            for (auto it = mBegin; it != mLast;)
            {
                // equal
                if (size_t(mLast - it) >= fromLen &&
                    detail::equal_nolength(const_iterator{it}, const_iterator{it + fromLen}, from.begin(), from.end()))
                {
                    result.append(to);
                    it += fromLen;
                }
                else
                {
                    result.push_back(*it);
                    ++it;
                }
            }

            return result;
        }

        inline SelfT remove_all(ViewT from)
        {
            size_t fromLen = from.size();

            assert(mLast >= mBegin);

            SelfT result;
            for (auto it = mBegin; it != mLast;)
            {
                // equal
                if (size_t(mLast - it) >= fromLen &&
                    detail::equal_nolength(const_iterator{it}, const_iterator{it + fromLen}, from.begin(), from.end()))
                {
                    it += fromLen;
                }
                else
                {
                    result.push_back(*it);
                    ++it;
                }
            }

            return result;
        }

        template <class LambdaT, class GroupT = std::invoke_result_t<LambdaT, value_type>>
        inline List<std::pair<GroupT, List<value_type, AllocatorT>>, AllocatorT> group_by(LambdaT &&pred)
        {
            return ulib::group_by(*this, std::forward<LambdaT>(pred));
        }

    private:
        inline void ConcreteReallocateMemory(size_t currentSizeInBytes, size_t reqSizeInBytes)
        {
            size_t oldSizeInBytes = currentSizeInBytes;
            size_t allocSizeInBytes = reqSizeInBytes;
            pointer oldBegin = mBegin;

            mEndB = (mLastB = mBeginB = (uchar *)ResourceT::Alloc(allocSizeInBytes)) + allocSizeInBytes;
            mLastB += oldSizeInBytes;

            if constexpr (IsRawMovable())
            {
                ::memcpy(mBegin, oldBegin, oldSizeInBytes);
            }
            else
            {
                pointer srcIt = oldBegin;
                for (auto it = mBegin; it != mLast; ++it, ++srcIt)
                    new (it) T(std::move(*srcIt));
            }

            ResourceT::Free(oldBegin);
        }

        inline void ReallocateMemory(size_t currentSizeInBytes, size_t additionalSizeInBytes)
        {
            ConcreteReallocateMemory(currentSizeInBytes, currentSizeInBytes + additionalSizeInBytes);
        }

        inline void ReallocIfEnd()
        {
            if (mLast == mEnd)
            {
                size_t sizeInBytes = SizeInBytes();
                ReallocateMemory(sizeInBytes, std::max(sizeInBytes, C_STEP * sizeof(T)));
            }

            assert(mLast < mEnd && "it seems like ReallocIfEnd was passed with errors in ReallocateMemory call");
        }

        inline void ReallocIfNeeded(size_t reqSizeInBytes)
        {
            size_t curr = SizeInBytes();
            if (curr < reqSizeInBytes)
            {
                ReallocateMemory(curr, reqSizeInBytes);
            }

            assert(!(SizeInBytes() < reqSizeInBytes) && "it seems like ReallocIfNeeded was passed with errors in ReallocateMemory call");
        }

        inline void RequestFreeSpaceB(size_t reqFreeSpace)
        {
            size_t curr = mEndB - mLastB;
            if (curr < reqFreeSpace)
            {
                ReallocateMemory(SizeInBytes(), reqFreeSpace);
            }

            assert(!(size_t(mEndB - mLastB) < reqFreeSpace) && "it seems like RequestFreeSpaceB was passed with errors in ReallocateMemory call");
        }

        inline void RequestFreeSpace(size_t reqFreeSpace) { RequestFreeSpaceB(reqFreeSpace * sizeof(T)); }

        inline void CopyConstructElementsB(pointer dest, const_pointer src, size_type bcount)
        {
            if (IsTrivial())
            {
                memcpy(dest, src, bcount);
            }
            else
            {
                auto e = pointer((uchar *)dest + bcount);
                auto s = src;
                for (auto it = dest; it != e; it++, s++)
                    new (it) T(*s);
            }
        }
        inline void CopyConstructElements(pointer dest, const_pointer src, size_type count)
        {
            return CopyConstructElementsB(dest, src, count * sizeof(T));
        }
        inline void CopyConstructElements(const_pointer first, const_pointer last, pointer dest)
        {
            CopyConstructElements(dest, first, (last - first));
        }

        inline void SetupViaCapacityB(size_type bcount) { mEndB = (mLastB = mBeginB = (uchar *)ResourceT::Alloc(bcount)) + bcount; }
        inline void SetupViaSizeB(size_type bcount) { mEndB = mLastB = (mBeginB = (uchar *)ResourceT::Alloc(bcount)) + bcount; }
        inline void SetupViaSizeAndConstructB(size_type bcount)
        {
            SetupViaSizeB(bcount);
            if constexpr (!IsTrivial())
                for (auto it = mBegin; it != mLast; it++)
                    new (it) T();
        }
        inline void SetupWithCopyConstructB(const_pointer src, size_type bcount)
        {
            SetupViaSizeB(bcount);
            CopyConstructElementsB(mBegin, src, bcount);
        }

        inline void SetupViaCapacity(size_type count) { SetupViaCapacityB(count * sizeof(T)); }
        inline void SetupWithCopyConstruct(const_pointer src, size_type count) { SetupWithCopyConstructB(src, count * sizeof(T)); }
        inline void SetupWithCopyConstruct(const_pointer first, const_pointer last) { SetupWithCopyConstructB(first, (last - first) * sizeof(T)); }
        inline void SetupViaSizeAndConstruct(size_type count) { return SetupViaSizeAndConstructB(count * sizeof(T)); }

        template <class IterT>
        inline void SetupWithCopyConstructWithIterators(IterT first, IterT last)
        {
            // if constexpr (std::is_pointer_v<IterT> || is_iterator_tag_v<IterT, std::random_access_iterator_tag>)
            // {
            //     size_t size = last - first;
            //     SetupViaSizeB(size * sizeof(T));

            //     memcpy(mBegin, &(*first), size * sizeof(T));
            // }
            // else
            {
                SetupViaCapacity(C_STEP);
                for (auto it = first; it != last; ++it)
                    PushBack(T{*it});
            }
        }

        inline SelfT AdditionImpl(ViewT right) const
        {
            SelfT result{args::Capacity(Size() + right.Size())};

            result.CopyConstructElements(mBegin, mLast, result.mBegin);
            result.CopyConstructElements(right.Begin().Raw(), right.End().Raw(), result.mBegin + Size());

            result.mLast = result.mEnd;

            return result;
        }

        // inline void CopyInit(const_pointer source, size_t dataSize)
        // {
        //     mEndB = mLastB = (mBeginB = (uchar *)ResourceT::Alloc(dataSize)) + dataSize;

        //     if constexpr (IsTrivial())
        //     {
        //         memcpy(mBeginB, source, dataSize);
        //     }
        //     else
        //     {
        //         T *ptr = mBegin;
        //         const T *ptr2 = source;

        //         for (; ptr != mLast; ptr++, ptr2++)
        //             new (ptr) T(*ptr2);
        //     }
        // }

        inline iterator Unbound(const_iterator it) { return pointer(it.raw()); }

        // template <class ListT>
        // inline void CopyInit(const ListT &source)
        // {
        //     CopyInit(source.mBegin, source.SizeInBytes());
        // }

        union {
            struct
            {
                T *mBegin;
                T *mLast;
                T *mEnd;
            };

            struct
            {
                uchar *mBeginB;
                uchar *mLastB;
                uchar *mEndB;
            };
        };
    };

    template <class Container>
    List(Container) -> List<typename Container::value_type>;

    template <class T, class AllocatorTy = DefaultAllocator>
    using list = List<T, AllocatorTy>;

    template <class AllocatorT>
    using BasicBuffer = List<uchar, AllocatorT>;
    using Buffer = BasicBuffer<DefaultAllocator>;
    using buffer = Buffer;

    // use for Cont<T*> where T* allocated as new T
    template <class ContT>
    void LoopDelete(const ContT &cont)
    {
        for (auto ptr : cont)
            delete ptr;
    }

    // use for Cont<T*> where T* allocated as new (buf) T
    template <class ContT>
    void LoopDestroy(const ContT &cont)
    {
        for (auto ptr : cont)
            ptr->~T();
    }
} // namespace ulib