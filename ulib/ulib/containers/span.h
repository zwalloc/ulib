#pragma once

#include <ulib/containers/filterview.h>
#include <ulib/containers/iterators/randomaccessiterator.h>
#include <ulib/containers/mapview.h>
#include <ulib/containers/splitview.h>
#include <ulib/containers/tags.h>

#include <ulib/types.h>
#include <ulib/typetraits/container.h>

#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace ulib
{
    static constexpr size_t npos = size_t(-1);

    namespace detail
    {
        template <class IterT>
        inline bool equal(IterT b0, IterT e0, IterT b1, IterT e1)
        {
            while (b0 != e0)
            {
                if (b1 == e1)
                    return false;

                if (*b0 != *b1)
                    return false;

                ++b0;
                ++b1;
            }

            return true;
        }

        template <class IterT>
        inline bool equal_nolength(IterT b0, IterT e0, IterT b1, IterT e1)
        {
            while (b0 != e0)
            {
                if (*b0 != *b1)
                    return false;

                ++b0;
                ++b1;
            }

            return true;
        }
    } // namespace detail

    template <class T>
    class ReversedSpan
    {
    public:
        using ReverseIterator = ReverseRandomAccessIterator<T>;
        using reverse_iterator = ReverseIterator;

        using ContainerTypeT = list_type_tag;
        using ContainerOwnershipT = view_ownership_tag;

        ReversedSpan(ReverseIterator rb, ReverseIterator re) noexcept : ms(rb), me(re) {}
        ~ReversedSpan() noexcept = default;

        inline ReverseIterator begin() const noexcept { return ms; }
        inline ReverseIterator end() const noexcept { return me; }

    private:
        ReverseIterator ms;
        ReverseIterator me;
    };

    template <class T>
    class Span
    {
    public:
        using SelfT = Span<T>;

        using element_type = T;
        using value_type = std::remove_cv_t<T>;

        using BufferViewT = Span<const uchar>;
        using ReverseT = ReversedSpan<const T>;
        using SplitViewT = SplitView<SelfT>;

        using ContainerTypeT = list_type_tag;
        using ContainerOwnershipT = view_ownership_tag;

        using SpanT = Span<T>;
        using ViewT = Span<const T>;

        using pointer = T *;
        using const_pointer = const T *;
        using reference = T &;
        using const_reference = const T &;
        using iterator = RandomAccessIterator<T>;
        using const_iterator = RandomAccessIterator<const T>;
        using reverse_iterator = ReverseRandomAccessIterator<T>;
        using const_reverse_iterator = ReverseRandomAccessIterator<const T>;
        using size_type = size_t;

        Span() noexcept { mBegin = mLast = nullptr; }
        Span(const SelfT &other) noexcept { mBegin = other.mBegin, mLast = other.mLast; }
        Span(pointer b, pointer e) noexcept { mBegin = b, mLast = e; }
        Span(pointer v, size_type count) noexcept { mBegin = v, mLast = v + count; }
        Span(iterator first, iterator last) noexcept { mBegin = first.raw(), mLast = last.raw(); }

        template <class K, enable_if_span_from_range_constructible_t<SelfT, K> = true>
        Span(K &&arr)
        {
            mBegin = std::data(arr);
            mLast = mBegin + std::size(arr);
        }

        template <size_type N>
        Span(T (&arr)[N]) noexcept
        {
            mBegin = &arr[0], mLast = &arr[N];
        }

        ~Span() noexcept = default;

        inline pointer Data() const { return mBegin; }
        inline iterator Begin() const { return mBegin; }
        inline iterator End() const { return mLast; }
        inline reverse_iterator ReverseBegin() const { return mLast - 1; }
        inline reverse_iterator ReverseEnd() const { return mBegin - 1; }
        inline ReverseT Reverse() const { return ReverseT{ReverseBegin(), ReverseEnd()}; }
        inline bool Empty() const { return mBegin == mLast; }
        inline size_type Size() const { return mLast - mBegin; }
        inline size_type Length() const { return mLast - mBegin; }
        inline size_type SizeInBytes() const { return size_type(mLast) - size_type(mBegin); }
        inline void RemovePrefix(size_type c) { mBegin += c; }
        inline void RemoveSuffix(size_type c) { mLast -= c; }
        inline reference At(size_type idx) const
        {
            if (idx >= Size())
                throw std::out_of_range{".at() out of range"};
            return mBegin[idx];
        }
        inline reference Front() const
        {
            assert(!Empty());
            return *Begin();
        }

        inline reference Back() const
        {
            assert(!Empty());
            return *(mLast - 1);
        }

        inline bool Compare(ViewT right) const { return std::equal(mBegin, mLast, right.Begin().Raw(), right.End().Raw()); }

        inline size_type Find(const_reference v, size_type pos = 0) const
        {
            auto it = std::find(mBegin + pos, mLast, v);
            return it == mLast ? npos : it - mBegin;

            // for (auto it = mBegin + pos; it != mLast; it++)
            // {
            //     if (*it == v)
            //         return pos;
            //     ++pos;
            // }

            // return npos;
        }

        inline size_type Find(ViewT v, size_type pos = 0) const
        {
            auto it = std::search(mBegin + pos, mLast, v.Begin().Raw(), v.End().Raw());
            return it == mLast ? npos : it - mBegin;

            // size_type msize = SizeInBytes() - pos * sizeof(value_type), vsize = v.SizeInBytes();
            // if (msize < vsize)
            //     return npos;

            // auto it = mBegin + pos;
            // while (true)
            // {
            //     if (memcmp(it, v.mBegin, vsize) == 0)
            //         return pos;

            //     ++pos;
            //     ++it;

            //     if (it == (const_pointer)((uchar *)mLast - vsize))
            //     {
            //         if (memcmp(it, v.mBegin, vsize) == 0)
            //             return pos;

            //         return npos;
            //     }
            // }

            // return npos;
        }

        inline size_type ReverseFind(const_reference v, size_type pos = 0) const
        {
            // auto re = mBegin + pos - 1;

            auto it = std::find(ReverseBegin(), reverse_iterator{mBegin + pos - 1}, v);
            return it == ReverseEnd() ? npos : it.base() - iterator(mBegin);

            // auto mend = mBegin + pos - 1;
            // auto mbegin = mLast - 1;

            // size_type rpos = Size() - 1;

            // for (auto it = mbegin; it != mend; it--)
            // {
            //     if (*it == v)
            //         return rpos;
            //     --rpos;
            // }

            // return npos;
        }

        inline size_type ReverseFind(ViewT v, size_type pos = 0) const
        {
            auto it = std::search(ReverseBegin(), reverse_iterator{mBegin + pos - 1}, v.ReverseBegin(), v.ReverseEnd());
            return it == ReverseEnd() ? npos : it.base() - iterator(mBegin) - v.Size() + 1;

            // size_type msize = SizeInBytes() - pos * sizeof(value_type), vsize = v.SizeInBytes();
            // if (msize < vsize)
            //     return false;

            // auto mend = mBegin + pos - 1;
            // auto mbegin = (const_pointer)((uchar *)mLast - vsize);
            // size_type rpos = Size() - v.Size();

            // for (auto it = mbegin; it != mend; it--)
            // {
            //     if (memcmp(it, v.mBegin, vsize) == 0)
            //         return rpos;
            //     --rpos;
            // }

            // return npos;
        }

        inline bool StartsWith(const_reference v) const
        {
            if (mBegin != mLast)
                return Front() == v;
            return false;
        }
        inline bool EndsWith(const_reference v) const
        {
            if (mBegin != mLast)
                return Back() == v;
            return false;
        }
        inline bool Contains(const_reference v) const
        {
            for (auto &obj : *this)
                if (obj == v)
                    return true;
            return false;
        }

        inline bool StartsWith(ViewT v) const
        {
            size_type msize = SizeInBytes(), vsize = v.SizeInBytes();
            if (msize < vsize)
                return false;

            return std::equal(v.Begin().Raw(), v.End().Raw(), mBegin);

            // return memcmp(mBegin, v.mBegin, vsize) == 0;
        }

        inline bool EndsWith(ViewT v) const
        {
            size_type msize = SizeInBytes(), vsize = v.SizeInBytes();
            if (msize < vsize)
                return false;

            return std::equal(v.Begin().Raw(), v.End().Raw(), (const_pointer)((uchar *)mLast - vsize));
            // return memcmp((const_pointer)((uchar *)mLast - vsize), v.mBegin, vsize) == 0;
        }

        inline bool Contains(ViewT v) const { return Find(v) != npos; }
        inline SelfT SubSpan(size_type pos, size_type n = npos) const
        {
            auto first = mBegin + pos;
            if (n == npos)
            {
                if (first > mLast)
                    throw std::out_of_range{"subspan out of range"};

                return SelfT{first, mLast};
            }
            else
            {
                auto last = first + n;
                if (last > mLast)
                    throw std::out_of_range{"subspan out of range"};

                return SelfT{first, last};
            }
        }

        inline SplitViewT Split(ViewT sep) const { return SplitViewT{*this, sep}; }
        inline BufferViewT Raw() const { return BufferViewT{(const uchar *)mBegin, (const uchar *)mLast}; }

        // operators

        inline SelfT &operator=(const SelfT &right) { return mBegin = right.mBegin, mLast = right.mLast, *this; }
        // inline SelfT &operator=(ViewT right) { return mBegin = right.mBegin, mLast = right.mLast, *this; }
        // template <class K, enable_if_range_compatible_t<SelfT, K> = true>
        // inline SelfT &operator=(const K &right)
        // {
        //     mBegin = right.data();
        //     mLast = mBegin + right.size();
        // }

        template <class K, std::enable_if_t<std::is_constructible_v<ViewT, K>, bool> = true>
        inline bool operator==(const K &right) const
        {
            return Compare(ViewT{right});
        }

        template <class K, std::enable_if_t<std::is_constructible_v<ViewT, K>, bool> = true>
        inline bool operator!=(const K &right) const
        {
            return !Compare(ViewT{right});
        }

        inline reference operator[](size_type idx) const { return mBegin[idx]; }

        // aliases

        inline pointer data() const { return Data(); }
        inline iterator begin() const { return mBegin; }
        inline iterator end() const { return mLast; }
        inline reverse_iterator rbegin() const { return mLast - 1; }
        inline reverse_iterator rend() const { return mBegin - 1; }
        inline ReverseT reverse() const { return Reverse(); }
        inline bool empty() const { return Empty(); }
        inline size_type size() const { return Size(); }
        inline size_type size_in_bytes() const { return SizeInBytes(); }
        inline size_type length() const { return Length(); }
        inline void remove_prefix(size_type c) const { mBegin += c; }
        inline void remove_suffix(size_type c) const { mLast -= c; }
        inline reference at(size_type idx) const { return At(idx); }
        inline reference front() const { return Front(); }
        inline reference back() const { return Back(); }
        inline bool compare(ViewT right) const { return Compare(right); }
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
        inline SplitViewT split(ViewT sep) const { return Split(sep); }
        inline BufferViewT raw() const { return Raw(); }

        inline SelfT lstrip(ViewT bytes)
        {
            auto it = mBegin;
            if (it != mLast)
            {
                for (auto bIt = bytes.mBegin; bIt != bytes.mLast;)
                {
                    if (*it == *bIt)
                    {
                        it++;
                        if (it == mLast)
                            return SelfT{it, mLast};

                        bIt = bytes.mBegin;
                    }
                    else
                    {
                        bIt++;
                    }
                }
            }

            return SelfT{it, mLast};
        }

        inline SelfT rstrip(ViewT bytes)
        {
            auto it = mBegin;
            if (it != mLast)
            {
                auto rIt = mLast - 1;
                auto eIt = it - 1;

                for (auto bIt = bytes.mBegin; bIt != bytes.mLast;)
                {
                    if (*rIt == *bIt)
                    {
                        rIt--;
                        if (rIt == eIt)
                            return SelfT{it, it};

                        bIt = bytes.mBegin;
                    }
                    else
                    {
                        bIt++;
                    }
                }

                return SelfT{it, rIt + 1};
            }

            return SelfT{it, it};
        }

        inline SelfT strip(ViewT bytes)
        {
            auto it = mBegin;
            if (it != mLast)
            {
                for (auto bIt = bytes.mBegin; bIt != bytes.mLast;)
                {
                    if (*it == *bIt)
                    {
                        it++;
                        if (it == mLast)
                            return SelfT{it, it};

                        bIt = bytes.mBegin;
                    }
                    else
                    {
                        bIt++;
                    }
                }

                auto rIt = mLast - 1;
                auto eIt = it - 1;

                for (auto bIt = bytes.mBegin; bIt != bytes.mLast;)
                {
                    if (*rIt == *bIt)
                    {
                        rIt--;
                        if (rIt == eIt)
                            return SelfT{it, it};

                        bIt = bytes.mBegin;
                    }
                    else
                    {
                        bIt++;
                    }
                }

                return SelfT{it, rIt + 1};
            }

            return SelfT{it, it};
        }

        template <class Pred, class... Args>
        auto map(Pred &&pred, Args &&...args) const
        {
            return CreateMapView<ViewT>(*this, std::forward<Pred>(pred), std::forward<Args>(args)...);
        }

        template <class Pred, class... Args>
        auto map(Pred &&pred, Args &&...args)
        {
            return CreateMapView<SelfT>(*this, std::forward<Pred>(pred), std::forward<Args>(args)...);
        }

        template <class Pred, class... Args>
        auto filter(Pred &&pred, Args &&...args) const
        {
            return CreateFilterView<ViewT>(*this, std::forward<Pred>(pred), std::forward<Args>(args)...);
        }

        template <class Pred, class... Args>
        auto filter(Pred &&pred, Args &&...args)
        {
            return CreateFilterView<SelfT>(*this, std::forward<Pred>(pred), std::forward<Args>(args)...);
        }

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

    private:
        pointer mBegin;
        pointer mLast;
    };

    template <class T>
    using SpanView = Span<const T>;

    template <class T>
    using span = Span<T>;

    template <class T>
    using span_view = SpanView<T>;

    using BufferView = SpanView<uchar>;
    using buffer_view = BufferView;

    // template<class T, class T1>
    // inline bool operator==(Span<T> left, Span<T1> right)
    // {
    //     return left.Compare(right);
    // }

    // template <class K, enable_if_range_compatible_t<SelfT, K> = true>
    // inline bool operator!=(const K &right) const
    // {
    //     return !Compare(ViewT{right});
    // }
    // inline bool operator!=(ViewT right) const { return !Compare(right); }

} // namespace ulib