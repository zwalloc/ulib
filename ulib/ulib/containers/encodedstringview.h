#pragma once

#include <ulib/config.h>
#include <ulib/cstrlen.h>
#include <ulib/encodings/length.h>
#include <ulib/span.h>

#include <ulib/typetraits/string.h>

#ifdef ULIB_STD_COMPATIBILITY
#include <string>
#include <string_view>
#endif

#include <filesystem>

#ifdef _WIN32
#ifdef _DEBUG
#include "cstr_debug.h"
#define ULIB_USE_C_STR_READ_CHECK_TPL
#endif
#endif

namespace ulib
{
    constexpr bool is_compiler_utf8() { return sizeof(u8"К") == 3 && sizeof("К") == 3; }

#ifndef ULIB_STRING_SKIP_UTF8_CHECK
    static_assert(is_compiler_utf8(),
                  "ulib/string.h requires your compiler to run in UTF-8 mode for some string conversions to work properly. Check compiler "
                  "documentation for the necessary flags and set them, or define ULIB_STRING_SKIP_UTF8_CHECK to ignore this error.");
#endif

    template <uint line>
    struct LineTag
    {
    };

#define ULIB_LINE_TAG LineTag<__LINE__>

    // --------------

    template <class EncodingT, class = void>
    struct parent_std_string
    {
        using type = missing_type;
    };

    template <class EncodingT>
    struct parent_std_string<EncodingT, std::void_t<typename EncodingT::ParentEncodingT::CharT>>
    {
        using type = std::basic_string<typename EncodingT::ParentEncodingT::CharT>;
    };

    template <class EncodingT>
    using parent_std_string_t = typename parent_std_string<EncodingT>::type;

    // ------------

    template <class EncodingT, class = void>
    struct parent_std_string_view
    {
        using type = missing_type;
    };

    template <class EncodingT>
    struct parent_std_string_view<EncodingT, std::void_t<typename EncodingT::ParentEncodingT::CharT>>
    {
        using type = std::basic_string_view<typename EncodingT::ParentEncodingT::CharT>;
    };

    template <class EncodingT>
    using parent_std_string_view_t = typename parent_std_string_view<EncodingT>::type;

    template <class CharT>
    const CharT *checked_c_str(const CharT *ch)
    {
#ifdef ULIB_USE_READ_CHECK_TPL
        return c_str_read_check_tpl<CharT>(ch);
#else
        return ch;
#endif
    }

    template <class EncodingTy, class CharTy = typename EncodingTy::CharT>
    class EncodedStringSpan
    {
    public:
        using SelfT = EncodedStringSpan<EncodingTy, CharTy>;
        using EncodingT = EncodingTy;
        using CharT = std::remove_cv_t<typename EncodingT::CharT>;
        using SpanT = Span<CharTy>;
        using ViewT = EncodedStringSpan<EncodingTy, const CharTy>;

        using Iterator = RandomAccessIterator<CharTy>;
        using ConstIterator = RandomAccessIterator<const CharTy>;
        using ReverseIterator = ReverseRandomAccessIterator<CharTy>;
        using ConstReverseIterator = ReverseRandomAccessIterator<const CharTy>;
        using ReverseT = ReversedSpan<const CharT>;
        using SplitViewT = SplitView<SelfT>;

        using element_type = CharTy;
        using value_type = CharT;
        using pointer = CharTy *;
        using const_pointer = const CharTy *;
        using reference = CharTy &;
        using const_reference = const CharTy &;
        using iterator = Iterator;
        using const_iterator = ConstIterator;
        using reverse_iterator = ReverseIterator;
        using const_reverse_iterator = ConstReverseIterator;
        using size_type = size_t;

        // ulib fields
        using ContainerTypeT = string_type_tag;
        using ContainerOwnershipT = view_ownership_tag;

        using ParentEncodingT = parent_encoding_t<EncodingT>;
        using ParentEncodingCharT = encoding_char_t<ParentEncodingT>;

        using ParentStdStringT = parent_std_string_t<EncodingT>;
        using ParentStdStringViewT = parent_std_string_view_t<EncodingT>;

        using StdStringT = std::basic_string<CharT>;
        using StdStringViewT = std::basic_string_view<CharT>;

        using OperatorParentStdStringT =
            undefined_if_missing_t<type_if_t<ParentStdStringT, !std::is_same_v<ParentStdStringT, StdStringT>>, ULIB_LINE_TAG>;
        using OperatorParentStdStringViewT =
            undefined_if_missing_t<type_if_t<ParentStdStringViewT, !std::is_same_v<ParentStdStringViewT, StdStringViewT>>, ULIB_LINE_TAG>;

        EncodedStringSpan() {}

        template <class T, class TEncodingT = literal_encoding_t<T>,
                  std::enable_if_t<std::is_same_v<EncodingT, TEncodingT> || std::is_same_v<EncodingT, parent_encoding_t<TEncodingT>>, bool> = true>
        EncodedStringSpan(const T *str) noexcept : mSpan((CharT *)str, cstrlen(checked_c_str(str)))
        {
        }
        EncodedStringSpan(const CharT *b, const CharT *e) noexcept : mSpan(b, e) {}
        EncodedStringSpan(ConstIterator b, ConstIterator e) noexcept : mSpan(b.ptr, e.ptr) {}
        EncodedStringSpan(const CharT *str, size_t size) noexcept : mSpan(str, size) {}

        EncodedStringSpan(const EncodedStringSpan<EncodingT> &source) : mSpan(source) {}

        template <class K, enable_if_span_from_range_constructible_t<SelfT, K> = true,
                  std::enable_if_t<!std::is_same_v<parent_encoding_t<std::remove_reference_t<K>>, EncodingT>, bool> = true>
        EncodedStringSpan(K &&str) : mSpan(str)
        {
        }

        template <class K, std::enable_if_t<std::is_same_v<typename std::remove_reference_t<K>::ParentEncodingT, EncodingT>, bool> = true>
        EncodedStringSpan(K &&str) : mSpan((CharT *)str.data(), str.size())
        {
        }

        ~EncodedStringSpan() noexcept = default;

        // functions

        inline pointer Data() { return mSpan.Data(); }
        inline pointer Data() const { return mSpan.Data(); }
        inline iterator Begin() const { return mSpan.Begin(); }
        inline iterator End() const { return mSpan.End(); }
        inline reverse_iterator ReverseBegin() const { return mSpan.ReverseBegin(); }
        inline reverse_iterator ReverseEnd() const { return mSpan.ReverseEnd(); }
        inline ReverseT Reverse() const { return ReverseT{ReverseBegin(), ReverseEnd()}; }
        inline bool Empty() const { return mSpan.Empty(); }
        inline size_type Size() const { return mSpan.Size(); }
        inline size_type Length() const { return CalcStringLength<EncodingT>(mSpan.Begin().Raw(), mSpan.End().Raw()); }
        inline size_type SizeInBytes() const { return mSpan.SizeInBytes(); }
        inline void RemovePrefix(size_type c) { mSpan.RemovePrefix(c); }
        inline void RemoveSuffix(size_type c) { mSpan.RemoveSuffix(c); }
        inline reference At(size_type idx) const { return mSpan.At(idx); }
        inline reference Front() const { return mSpan.Front(); }
        inline reference Back() const { return mSpan.Back(); }
        inline bool Compare(SelfT right) const { return mSpan.Compare(right); }
        inline size_type Find(const_reference v, size_type pos = 0) const { return mSpan.Find(v, pos); }
        inline size_type Find(SelfT v, size_type pos = 0) const { return mSpan.Find(v, pos); }
        inline size_type ReverseFind(const_reference v, size_type pos = 0) const { return mSpan.ReverseFind(v, pos); }
        inline size_type ReverseFind(SelfT v, size_type pos = 0) const { return mSpan.ReverseFind(v, pos); }
        inline bool StartsWith(const_reference v) const { return mSpan.StartsWith(v); }
        inline bool EndsWith(const_reference v) const { return mSpan.EndsWith(v); }
        inline bool Contains(const_reference v) const { return mSpan.Contains(v); }
        inline bool StartsWith(ViewT v) const { return mSpan.StartsWith(v); }
        inline bool EndsWith(ViewT v) const { return mSpan.EndsWith(v); }
        inline bool Contains(ViewT v) const { return mSpan.Contains(v); }
        inline SelfT SubString(size_type pos, size_type n = npos) const { return SelfT(mSpan.SubSpan(pos, n)); }
        inline SplitViewT Split(ViewT sep) const { return SplitViewT{*this, sep}; }
        inline BufferView Raw() const { return mSpan.Raw(); }

        // operators

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

        // template <class K, enable_if_range_compatible_t<SelfT, K> = true>
        // inline bool operator==(const K &right) const
        // {
        //     return mSpan.Compare(SpanT{right});
        // }
        // inline bool operator==(const CharT *right) const { return mSpan.Compare(SpanT{right, cstrlen(right)}); }
        // inline bool operator==(SelfT right) const { return mSpan.Compare(right); }

        // template <class K, enable_if_range_compatible_t<SelfT, K> = true>
        // inline bool operator!=(const K &right) const
        // {
        //     return !mSpan.Compare(SpanT{right});
        // }
        // inline bool operator!=(const CharT *right) const { return !mSpan.Compare(SpanT{right, cstrlen(right)}); }
        // inline bool operator!=(SelfT right) const { return !mSpan.Compare(right); }

        inline reference operator[](size_type idx) const { return mSpan[idx]; }

#ifdef ULIB_STD_COMPATIBILITY

        operator StdStringViewT() const { return std::basic_string_view<CharT>(mSpan.Data(), mSpan.Size()); }
        operator StdStringT() const { return std::basic_string<CharT>(mSpan.Data(), mSpan.Size()); }

#ifdef __cpp_char8_t
        // template <class CurrentCharT = typename EncodingT::CharStd, std::enable_if_t<std::is_same_v<ParentEncodingT,
        // MultibyteEncoding>, bool> = true>
        operator OperatorParentStdStringViewT() const { return OperatorParentStdStringViewT((ParentEncodingCharT *)mSpan.Data(), mSpan.Size()); }
        operator OperatorParentStdStringT() const { return OperatorParentStdStringT((ParentEncodingCharT *)mSpan.Data(), mSpan.Size()); }

#endif
#endif

        operator std::filesystem::path() const { return std::filesystem::path{mSpan.begin(), mSpan.end()}; }

        // aliases

        inline pointer data() const { return Data(); }
        inline iterator begin() const { return Begin(); }
        inline iterator end() const { return End(); }
        inline reverse_iterator rbegin() const { return ReverseBegin(); }
        inline reverse_iterator rend() const { return ReverseEnd(); }
        inline ReverseT reverse() const { return Reverse(); }
        inline bool empty() const { return Empty(); }
        inline size_type size() const { return Size(); }
        inline size_type size_in_bytes() const { return SizeInBytes(); }
        inline size_type length() const { return Length(); }
        inline void remove_prefix(size_type c) { RemovePrefix(c); }
        inline void remove_suffix(size_type c) { RemoveSuffix(c); }
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
        inline SelfT substr(size_type pos, size_type n = npos) const { return SubString(pos, n); }
        inline SplitViewT split(ViewT sep) const { return Split(sep); }
        inline BufferView raw() const { return Raw(); }
        inline SelfT lstrip(ViewT chars) { return SelfT(mSpan.lstrip(chars.mSpan)); }
        inline SelfT rstrip(ViewT chars) { return SelfT(mSpan.rstrip(chars.mSpan)); }
        inline SelfT strip(ViewT chars) { return SelfT(mSpan.strip(chars.mSpan)); }

    private:
        SpanT mSpan;
    };

    template <class EncodingTy>
    using EncodedStringView = EncodedStringSpan<EncodingTy, const typename EncodingTy::CharT>;

    template <class EncodingT, class CharT>
    inline bool operator==(const CharT *left, ulib::EncodedStringView<EncodingT> str)
    {
        return str == left;
    }

    template <class EncodingT, class CharT>
    inline bool operator!=(const CharT *left, ulib::EncodedStringView<EncodingT> str)
    {
        return str != left;
    }

} // namespace ulib
