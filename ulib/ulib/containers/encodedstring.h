#pragma once

#include <ulib/config.h>
#include <ulib/cstrlen.h>
#include <ulib/types.h>

#include <ulib/encodings/length.h>

#include <ulib/containers/args.h>
#include <ulib/containers/encodedstringview.h>
#include <ulib/containers/tags.h>

#include <ulib/allocators/defaultallocator.h>
#include <ulib/resources/resource.h>

#include <ulib/typetraits/container.h>
#include <ulib/typetraits/literalencoding.h>
#include <ulib/typetraits/string.h>

#include <cstring>
#include <ulib/list.h>

#include <ulib/typetraits/typeordefault.h>

#ifdef min
#undef min
#endif

namespace ulib
{
    template <class EncodingTy, class AllocatorT>
    class EncodedString;

    // ------------

    template <class EncodingT, class AllocatorT, class = void>
    struct parent_encoded_string
    {
        using type = missing_type;
    };

    template <class EncodingT, class AllocatorT>
    struct parent_encoded_string<EncodingT, AllocatorT, std::void_t<typename EncodingT::ParentEncodingT>>
    {
        using type = EncodedString<typename EncodingT::ParentEncodingT, AllocatorT>;
    };

    template <class EncodingT, class AllocatorT>
    using parent_encoded_string_t = typename parent_encoded_string<EncodingT, AllocatorT>::type;

    // ------------

    template <class EncodingT, class = void>
    struct parent_encoded_string_view
    {
        using type = missing_type;
    };

    template <class EncodingT>
    struct parent_encoded_string_view<EncodingT, std::void_t<typename EncodingT::ParentEncodingT::CharT>>
    {
        using type = EncodedStringSpan<typename EncodingT::ParentEncodingT, const typename EncodingT::ParentEncodingT::CharT>;
    };

    template <class EncodingT>
    using parent_encoded_string_view_t = typename parent_encoded_string_view<EncodingT>::type;

    // -----------

    template <class EncodingTy, class AllocatorTy = DefaultAllocator>
    class EncodedString
    {
    public:
        static_assert(std::is_trivial_v<typename EncodingTy::CharT>, "CharT for EncodingString must be a trivial type");

        using EncodingT = EncodingTy;
        using AllocatorT = AllocatorTy;
        using SelfT = EncodedString<EncodingT, AllocatorT>;
        using SelfRefT = SelfT &;
        using CharT = typename EncodingT::CharT;
        using ListT = List<CharT, AllocatorT>;

        // using BaseT = Resource<AllocatorT>;
        // using ResourceT = Resource<AllocatorT>;

        using AllocatorParams = typename AllocatorT::Params;
        using ViewT = EncodedStringView<EncodingT>;

        using Iterator = RandomAccessIterator<CharT>;
        using ConstIterator = RandomAccessIterator<const CharT>;
        using ReverseIterator = ReverseRandomAccessIterator<CharT>;
        using ConstReverseIterator = ReverseRandomAccessIterator<const CharT>;
        using ReverseT = ReversedSpan<const CharT>;
        using SplitViewT = SplitView<ViewT>;

        using ParentEncodingT = parent_encoding_t<EncodingT>;
        using ParentEncodingCharT = encoding_char_t<ParentEncodingT>;

        using ParentStringT = parent_encoded_string_t<EncodingT, AllocatorT>;
        using ParentStringViewT = parent_encoded_string_view_t<EncodingT>;
        using ParentStdStringT = parent_std_string_t<EncodingT>;
        using ParentStdStringViewT = parent_std_string_view_t<EncodingT>;

        using StdStringT = std::basic_string<CharT>;
        using StdStringViewT = std::basic_string_view<CharT>;

        using OperatorParentEncodedStringT = undefined_if_missing_t<type_if_t<ParentStringT, !std::is_same_v<ParentStringT, SelfT>>, ULIB_LINE_TAG>;
        using OperatorParentEncodedStringViewT =
            undefined_if_missing_t<type_if_t<ParentStringViewT, !std::is_same_v<ParentStringViewT, ViewT>>, ULIB_LINE_TAG>;

        using OperatorParentStdStringT =
            undefined_if_missing_t<type_if_t<ParentStdStringT, !std::is_same_v<ParentStdStringT, StdStringT>>, ULIB_LINE_TAG>;
        using OperatorParentStdStringViewT =
            undefined_if_missing_t<type_if_t<ParentStdStringViewT, !std::is_same_v<ParentStdStringViewT, StdStringViewT>>, ULIB_LINE_TAG>;

        using SpanT = Span<CharT>;

        using value_type = CharT;
        using pointer = value_type *;
        using const_pointer = const value_type *;
        using reference = value_type &;
        using const_reference = const value_type &;
        using iterator = Iterator;
        using const_iterator = ConstIterator;
        using reverse_iterator = ReverseIterator;
        using const_reverse_iterator = ConstReverseIterator;
        using size_type = size_t;

        // ulib fields
        using ContainerTypeT = string_type_tag;
        using ContainerOwnershipT = store_ownership_tag;

        constexpr static size_t C_STEP = 16;
        constexpr static size_t M_STEP = sizeof(CharT) * C_STEP;

        // constructors

        inline EncodedString(AllocatorParams al = {}) : m(al) {}

        template <class T, class TEncodingT = literal_encoding_t<T>,
                  std::enable_if_t<std::is_same_v<EncodingT, TEncodingT> || std::is_same_v<EncodingT, parent_encoding_t<TEncodingT>> ||
                                       std::is_same_v<parent_encoding_t<EncodingT>, TEncodingT>,
                                   bool> = true>
        inline EncodedString(const T *str, AllocatorParams al = {}) : m((CharT *)str, cstrlen(checked_c_str(str)), al)
        {
        }

        inline EncodedString(const CharT *b, const CharT *e, AllocatorParams al = {}) : m(b, e, al) {}
        inline EncodedString(const CharT *str, size_t size, AllocatorParams al = {}) : m(str, size, al) {}

        template <class K, enable_if_container_from_range_constructible_t<SelfT, K> = true,
                  std::enable_if_t<!std::is_same_v<parent_encoding_t<std::remove_reference_t<K>>, EncodingT>, bool> = true>
        inline EncodedString(const K &cont, AllocatorParams al = {}) : m(cont, al)
        {
        }

        template <class K, std::enable_if_t<std::is_same_v<typename std::remove_reference_t<K>::ParentEncodingT, EncodingT>, bool> = true>
        EncodedString(const K &str) : m((CharT *)str.data(), str.size())
        {
        }

        inline explicit EncodedString(size_t size, AllocatorParams al = {}) : m(size, al) {}
        inline explicit EncodedString(args::Capacity capacity, AllocatorParams al = {}) : m(capacity, al) {}
        inline EncodedString(const SelfT &other) : m(other.m) {}
        inline EncodedString(SelfT &&other) : m(std::move(other.m)) {}
        inline EncodedString(ListT &&other) : m(std::move(other)) {}
        inline ~EncodedString() = default;

        // function definitions
        inline ViewT ToView() const { return ViewT{m}; }
        inline const_pointer RawData() const { return m.Data(); }
        inline const_pointer Data() const { return MarkZeroEndConst(), m.Data(); }
        inline const_iterator Begin() const { return m.Begin(); }
        inline const_iterator End() const { return m.End(); }
        inline const_reverse_iterator ReverseBegin() const { return m.ReverseBegin(); }
        inline const_reverse_iterator ReverseEnd() const { return m.ReverseEnd(); }
        inline ReverseT Reverse() const { return ReverseT{ReverseBegin(), ReverseEnd()}; }
        inline bool Empty() const { return m.Empty(); }
        inline size_type Size() const { return m.Size(); }
        inline size_type Length() const { return ToView().Length(); }
        inline size_type SizeInBytes() const { return m.SizeInBytes(); }
        inline const_reference At(size_type idx) const { return m.At(idx); }
        inline const_reference Front() const { return m.Front(); }
        inline const_reference Back() const { return m.Back(); }
        inline bool Compare(ViewT right) const { return m.Compare(right); }
        inline size_type Find(const_reference v, size_type pos = 0) const { return m.Find(v, pos); }
        inline size_type Find(ViewT v, size_type pos = 0) const { return m.Find(v, pos); }
        inline size_type ReverseFind(const_reference v, size_type pos = 0) const { return m.ReverseFind(v, pos); }
        inline size_type ReverseFind(ViewT v, size_type pos = 0) const { return m.ReverseFind(v, pos); }
        inline bool StartsWith(const_reference v) const { return m.StartsWith(v); }
        inline bool EndsWith(const_reference v) const { return m.EndsWith(v); }
        inline bool Contains(const_reference v) const { return m.Contains(v); }
        inline bool StartsWith(ViewT v) const { return m.StartsWith(v); }
        inline bool EndsWith(ViewT v) const { return m.EndsWith(v); }
        inline bool Contains(ViewT v) const { return m.Contains(v); }
        inline ViewT SubString(size_type pos, size_type n = npos) const { return ToView().SubString(pos, n); }
        // inline bool CompareIgnoreCase(ViewT right) const {}

        inline pointer RawData() { return m.Data(); }
        inline pointer Data() { return MarkZeroEndConst(), m.Data(); }
        inline iterator Begin() { return m.Begin(); }
        inline iterator End() { return m.End(); }
        inline reverse_iterator ReverseBegin() { return m.ReverseBegin(); }
        inline reverse_iterator ReverseEnd() { return m.ReverseEnd(); }
        inline size_type Capacity() const { return m.Capacity(); }
        inline size_type Available() const { return m.Available(); }
        inline size_type CapacityInBytes() const { return m.CapacityInBytes(); }
        inline size_type AvailableInBytes() const { return m.AvailableInBytes(); }
        // inline void SetSize(size_t newSize) { ; } // TODO: DO
        inline void PushBack(CharT ch) { return m.PushBack(ch); }
        inline void Reserve(size_t s) { m.Reserve(s); }
        inline void Resize(size_t s) { m.Resize(s); }
        inline void Resize(size_t s, CharT ch) { m.Resize(s, ch); }
        inline iterator Erase(const_iterator it) { return m.Erase(it); }
        inline iterator Erase(size_type i) { return m.Erase(i); }
        inline iterator Erase(const_iterator first, const_iterator last) { return m.Erase(first, last); }
        inline SelfT &Erase(size_type index = 0, size_type count = npos) { return m.Erase(index, count), *this; }
        inline size_type GetIndex(const_iterator it) const { return m.GetIndex(it); }
        inline iterator FastErase(const_iterator it) { return m.FastErase(it); }
        inline void PopBack() { return m.PopBack(); }
        inline void Pop() { return m.Pop(); }
        inline void MarkZeroEnd() { MarkZeroEndImpl(); }
        inline void Clear() { m.Clear(); }
        inline reference At(size_t i) { return m.At(i); }
        inline reference Front() { return m.Front(); }
        inline reference Back() { return m.Back(); }

        template <class K, enable_if_range_compatible_t<SelfT, K>>
        inline void Assign(K right)
        {
            m.Assign(right);
        }
        inline void Assign(const SelfT &source) { m.Assign(source); }
        inline void Assign(SelfT &&source) { m.Assign(std::move(source.m)); }

        inline SelfT &Append(ViewT right) { return m.Append(right), *this; }
        inline SplitViewT Split(ViewT sep) const { return SplitViewT{*this, sep}; }
        inline BufferView Raw() const { return m.Raw(); }

        inline iterator begin() { return Begin(); }
        inline iterator end() { return End(); }
        inline size_type available() const { return Available(); }
        inline size_type available_in_bytes() const { return AvailableInBytes(); }
        inline size_type capacity() const { return Capacity(); }
        inline size_type capacity_in_bytes() const { return CapacityInBytes(); }
        inline pointer raw_data() { return RawData(); }
        inline const_pointer raw_data() const { return RawData(); }
        inline pointer data() { return Data(); }
        inline iterator erase(const_iterator it) { return Erase(it); }
        // inline iterator erase(size_type i) { return Erase(i); }
        inline iterator erase(const_iterator first, const_iterator last) { return Erase(first, last); }
        inline SelfT &erase(size_type index = 0, size_type count = npos) { return Erase(index, count); }
        inline void push_back(const_reference o) { PushBack(o); }
        inline void push_back(CharT &&o) { PushBack(std::move(o)); }
        inline void pop_back() { PopBack(); }
        inline void reserve(size_type s) { Reserve(s); }
        inline void resize(size_type s) { Resize(s); }
        inline void resize(size_type s, CharT ch) { Resize(s, ch); }
        inline const_pointer c_str() const { return Data(); }
        inline void clear() { Clear(); }
        inline reference at(size_type i) { return At(i); };
        inline reference front() { return Front(); }
        inline reference back() { return Back(); }
        inline void assign(ViewT right) { Assign(right); }
        inline void assign(SelfT &&source) { Assign(std::move(source.m)); }
        inline SelfT &append(ViewT right) { return Append(right); }

        inline const_pointer data() const { return Data(); }
        inline const_iterator begin() const { return Begin(); }
        inline const_iterator end() const { return End(); }
        inline const_reverse_iterator rbegin() const { return ReverseBegin(); }
        inline const_reverse_iterator rend() const { return ReverseEnd(); }
        inline ReverseT reverse() const { return Reverse(); }
        inline bool empty() const { return Empty(); }
        inline size_type size() const { return Size(); }
        inline size_type size_in_bytes() const { return SizeInBytes(); }
        inline size_type length() const { return Length(); }
        inline const_reference at(size_type idx) const { return At(idx); }
        inline const_reference front() const { return Front(); }
        inline const_reference back() const { return Back(); }
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
        inline ViewT substr(size_type pos, size_type n = npos) const { return SubString(pos, n); }
        inline SplitViewT split(ViewT sep) const { return Split(sep); }
        inline BufferView raw() const { return Raw(); }
        // operators overloads

        inline reference operator[](size_t i) { return m[i]; }
        inline const_reference operator[](size_t i) const { return m[i]; }

        inline SelfT operator+(ViewT right) const { return SelfT(std::move(m + right)); }
        inline SelfT &operator+=(ViewT right) { return Append(right); }

        inline void operator=(const SelfT &right) { Assign(right); }
        inline void operator=(SelfT &&right) { Assign(std::move(right)); }

        inline bool operator==(ViewT right) const { return Compare(right); }
        inline bool operator!=(ViewT right) const { return !Compare(right); }

        inline SelfT operator*(size_t c) const
        {
            SelfT result;
            for (size_t i = 0; i != c; i++)
                result.append(*this);
            return result;
        }

        inline bool operator<(ViewT right) const { return LowerThanImpl(right.begin().raw(), right.end().raw()); }
        inline bool operator>(ViewT right) const { return !LowerThanImpl(right.begin().raw(), right.end().raw()) && *this != right; }

        inline SelfT replace(ViewT from, ViewT to) { return SelfT(std::move(m.replace(from, to))); }
        inline SelfT remove_all(ViewT from) { return SelfT(std::move(m.remove_all(from))); }

        template <class LambdaT, class GroupT = std::invoke_result_t<LambdaT, value_type>>
        inline List<std::pair<GroupT, List<value_type, AllocatorT>>, AllocatorT> group_by(LambdaT &&pred)
        {
            return m.group_by(std::forward<LambdaT>(pred));
        }
        // inline bool operator<(const CharT *right) const { return LowerThanImpl(right); }

        // inline bool operator==(const CharT *right) const { return Compare(right); }
        // inline bool operator!=(const CharT *right) const { return !Compare(right); }

        // template <class StringT, class SCharT = typename StringT::value_type, std::enable_if_t<!is_std_basic_string_view_v<StringT>, bool> = true>
        // inline bool operator==(const StringT &right) const
        // {
        //     return Compare(right);
        // }

        // template <class StringT, class SCharT = typename StringT::value_type, std::enable_if_t<!is_std_basic_string_view_v<StringT>, bool> = true>
        // inline bool operator!=(const StringT &right) const
        // {
        //     return !Compare(right);
        // }

        // operator OperatorParentEncodedStringT() const { return ParentStringT((ParentEncodingCharT *)m.Begin().Raw(), (ParentEncodingCharT
        // *)m.End().Raw()); } operator OperatorParentEncodedStringViewT() const { return ParentStringViewT((ParentEncodingCharT *)m.Begin().Raw(),
        // (ParentEncodingCharT *)m.End().Raw()); }

        // ----------------

        operator StdStringViewT() const { return StdStringViewT(m.Data(), m.Size()); }
        operator StdStringT() const { return StdStringT(m.Data(), m.Size()); }

        operator OperatorParentStdStringViewT() const { return OperatorParentStdStringViewT((ParentEncodingCharT *)m.Data(), m.Size()); }
        operator OperatorParentStdStringT() const { return OperatorParentStdStringT((ParentEncodingCharT *)m.Data(), m.Size()); }

        // operator OperatorParentStdStringViewT() const
        // {
        //     return std::basic_string_view<ParentEncodingCharT>((ParentEncodingCharT *)m.Data(), m.Size());
        // }

        // operator OperatorParentStdStringT() const { return std::basic_string<ParentEncodingCharT>((ParentEncodingCharT *)m.Begin().Raw(),
        // m.Size()); }

        operator std::filesystem::path() const { return std::filesystem::path{m.begin(), m.end()}; }

    protected:
        void MarkZeroEndImpl()
        {
            if (m.size() == m.capacity())
            {
                m.Reserve(m.size() + 1);
            }

            *m.end() = 0;
        }

        inline void MarkZeroEndConst() const { ((SelfT *)this)->MarkZeroEndImpl(); }

        inline bool LowerThanImpl(const CharT *right, const CharT *rightEnd) const
        {
            assert(right);
            assert(right < rightEnd);

            size_t rightSizeInBytes = size_t((uchar *)rightEnd - (uchar *)right);
            size_t minSize = std::min(SizeInBytes(), rightSizeInBytes);

            int result = memcmp(m.data(), right, minSize);
            if (result == 0)
                return minSize != rightSizeInBytes;

            return result < 0;
        }

        inline bool LowerThanImpl(const CharT *right) const
        {
            assert(right);

            return LowerThanImpl(right, right + cstrlen(right));
        }

        ListT m;
    };

// c++ < 20
#if not __cplusplus >= 202002L

#endif

    template <class CharT, class EncodingT = ulib::literal_encoding_t<CharT>, class AllocatorT>
    inline bool operator==(const CharT *const left, const ulib::EncodedString<EncodingT, AllocatorT> &right)
    {
        return right.Compare(left);
    }

    template <class CharT, class EncodingT = ulib::literal_encoding_t<CharT>, class AllocatorT>
    inline bool operator!=(const CharT *const left, const ulib::EncodedString<EncodingT, AllocatorT> &right)
    {
        return !right.Compare(left);
    }

} // namespace ulib

template <class EncodingT, class AllocatorT>
inline ulib::EncodedString<EncodingT, AllocatorT> operator+(ulib::EncodedStringSpan<EncodingT> left,
                                                            const ulib::EncodedString<EncodingT, AllocatorT> &right)
{
    ulib::EncodedString<EncodingT, AllocatorT> result(left);
    result += right;
    return result;
}

template <class EncodingT>
inline ulib::EncodedString<EncodingT> operator+(ulib::EncodedStringView<EncodingT> left, ulib::EncodedStringView<EncodingT> right)
{
    ulib::EncodedString<EncodingT> result(left);
    result += right;
    return result;
}

template <class EncodingT>
inline ulib::EncodedString<EncodingT> operator*(ulib::EncodedStringView<EncodingT> left, size_t right)
{
    return ulib::EncodedString<EncodingT>{left} * right;
}

template <class CharT, class EncodingT = ulib::literal_encoding_t<CharT>, class AllocatorT>
inline ulib::EncodedString<EncodingT, AllocatorT> operator+(const CharT *const left, const ulib::EncodedString<EncodingT, AllocatorT> &right)
{
    ulib::EncodedString<EncodingT> result{left};
    result += right;
    return result;
}

template <class CharT, class EncodingT = ulib::literal_encoding_t<CharT>>
inline ulib::EncodedString<EncodingT> operator+(const CharT *const left, ulib::EncodedStringView<EncodingT> right)
{
    ulib::EncodedString<EncodingT> result(left);
    result += right;
    return result;
}

namespace std
{
    template <class EncodingT, class AllocatorT, class AllocT>
    inline bool operator==(const std::basic_string<typename EncodingT::CharT, std::char_traits<typename EncodingT::CharT>, AllocT> &str,
                           const ulib::EncodedString<EncodingT, AllocatorT> &str1)
    {
        return str1.Compare(str);
    }

    template <class EncodingT, class AllocatorT, class AllocT>
    inline bool operator!=(const std::basic_string<typename EncodingT::CharT, std::char_traits<typename EncodingT::CharT>, AllocT> &str,
                           const ulib::EncodedString<EncodingT, AllocatorT> &str1)
    {
        return !str1.Compare(str);
    }
} // namespace std

/*
#ifdef ULIB_STD_COMPATIBILITY
template<class EncodingT, class AllocatorT>
inline bool operator==(const std::basic_string<typename EncodingT::CharT>& str, const ulib::EncodedString<EncodingT,
AllocatorT>& ustr)
{
    return ustr == str;
}
#endif
*/

// inline bool CompareImpl(SpanT right) const { return SpanT{mBegin, mLast}.Compare(right); }

// inline void AssignImpl(SelfT &&source)
// {
//     ResourceT::Free(mBeginB);

//     ResourceT &rs = *this;
//     rs = std::move(source);

//     mBegin = source.mBegin;
//     mLast = source.mLast;
//     mEnd = source.mEnd;

//     source.mBegin = nullptr;
// }

// inline void AssignImpl(const CharT *str, size_t sizeInBytes)
// {
//     size_t allocSize = sizeInBytes;
//     size_t requiredSize = sizeInBytes;

//     if (CapacityInBytes() < requiredSize)
//     {
//         ResourceT::Free(mBegin);
//         mEndB = (mBeginB = rawptr_t(ResourceT::Alloc(allocSize))) + allocSize;
//     }

//     mLastB = mBeginB + requiredSize;
//     memcpy(mBegin, str, requiredSize);
// }

// inline void AppendImpl(const CharT *right, size_t rightSizeInBytes)
// {
//     size_t sizeInBytes = SizeInBytes();
//     size_t reqSize = sizeInBytes + rightSizeInBytes;
//     if (Capacity() < reqSize)
//     {
//         ReallocateMemoryWithRight(sizeInBytes, reqSize, right, rightSizeInBytes);
//     }
//     else
//     {
//         memcpy(mLast, right, rightSizeInBytes);
//         mLast = mBegin + reqSize;
//     }
// }

// inline void AppendImpl(const CharT *right) { AppendImpl(right, cstrlen(right)); }

// inline SelfT AdditionImpl(const CharT *right, size_t rightSizeInBytes) const
// {
//     size_t sizeInBytes = SizeInBytes();
//     size_t reqSize = sizeInBytes + rightSizeInBytes;

//     SelfT result{args::Capacity(reqSize)};
//     result.InitSummary(this->mBegin, sizeInBytes, right, rightSizeInBytes, reqSize);

//     return result;
// }

// inline SelfT AdditionImpl(const CharT *right) const { return AdditionImpl(right, cstrlen(right)); }
