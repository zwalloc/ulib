#pragma once

#include "baseiterator.h"
#include <iterator>
#include <type_traits>

namespace ulib
{
    template <class T>
    class RandomAccessIterator : public BaseIterator<std::remove_cv_t<T>, std::random_access_iterator_tag>
    {
    public:
        using SelfT = RandomAccessIterator<T>;
        using SelfNoConstT = RandomAccessIterator<std::remove_cv_t<T>>;
        using SelfConstT = RandomAccessIterator<const std::remove_cv_t<T>>;

        inline RandomAccessIterator() {}
        inline RandomAccessIterator(T *p) : ptr(p) {}

        inline RandomAccessIterator(const SelfConstT &it) : ptr((T *)it.ptr) {}
        inline RandomAccessIterator(const SelfNoConstT &it) : ptr((T *)it.ptr) {}

        inline T &operator*() const { return *ptr; }
        inline SelfT &operator++() { return ++ptr, *this; }
        inline SelfT &operator++(int) { return ptr++, *this; }
        inline SelfT &operator--() { return --ptr, *this; }
        inline SelfT &operator--(int) { return ptr--, *this; }

        inline SelfT operator+(size_t k) const { return ptr + k; }
        inline SelfT operator-(size_t k) const { return ptr - k; }
        inline SelfT operator+=(size_t k) { return ptr += k; }
        inline SelfT operator-=(size_t k) { return ptr -= k; }

        inline bool operator>(SelfNoConstT k) const { return ptr > k.ptr; }
        inline bool operator<(SelfNoConstT k) const { return ptr < k.ptr; }
        inline bool operator<=(SelfNoConstT k) const { return ptr <= k.ptr; }
        inline bool operator>=(SelfNoConstT k) const { return ptr >= k.ptr; }
        inline bool operator==(SelfNoConstT k) const { return ptr == k.ptr; }
        inline bool operator!=(SelfNoConstT k) const { return ptr != k.ptr; }
        inline size_t operator-(SelfNoConstT k) const { return ptr - k.ptr; }

        inline bool operator>(SelfConstT k) const { return ptr > k.ptr; }
        inline bool operator<(SelfConstT k) const { return ptr < k.ptr; }
        inline bool operator<=(SelfConstT k) const { return ptr <= k.ptr; }
        inline bool operator>=(SelfConstT k) const { return ptr >= k.ptr; }
        inline bool operator==(SelfConstT k) const { return ptr == k.ptr; }
        inline bool operator!=(SelfConstT k) const { return ptr != k.ptr; }
        inline size_t operator-(SelfConstT k) const { return ptr - k.ptr; }

        inline T *operator->() const { return ptr; }

        inline T *Raw() { return ptr; }
        inline T *raw() { return ptr; }

        T *ptr;
    };

    template <class T>
    class ReverseRandomAccessIterator : public BaseIterator<std::remove_cv_t<T>, std::random_access_iterator_tag>
    {
    public:
        using SelfT = ReverseRandomAccessIterator<T>;
        using SelfNoConstT = ReverseRandomAccessIterator<std::remove_cv_t<T>>;
        using SelfConstT = ReverseRandomAccessIterator<const std::remove_cv_t<T>>;
        using BaseT = RandomAccessIterator<T>;

        inline ReverseRandomAccessIterator() {}
        inline ReverseRandomAccessIterator(T *p) : ptr(p) {}

        inline ReverseRandomAccessIterator(const SelfConstT &it) : ptr(it.ptr) {}
        inline ReverseRandomAccessIterator(const SelfNoConstT &it) : ptr(it.ptr) {}

        inline T &operator*() const { return *ptr; }
        inline SelfT &operator++() { return --ptr, *this; }
        inline SelfT &operator++(int) { return ptr--, *this; }
        inline SelfT &operator--() { return ++ptr, *this; }
        inline SelfT &operator--(int) { return ptr++, *this; }

        inline SelfT operator+(size_t k) const { return ptr - k; }
        inline SelfT operator-(size_t k) const { return ptr + k; }
        inline SelfT operator+=(size_t k) { return ptr -= k; }
        inline SelfT operator-=(size_t k) { return ptr += k; }

        inline bool operator>(SelfNoConstT k) const { return ptr < k.ptr; }
        inline bool operator<(SelfNoConstT k) const { return ptr > k.ptr; }
        inline bool operator<=(SelfNoConstT k) const { return ptr >= k.ptr; }
        inline bool operator>=(SelfNoConstT k) const { return ptr <= k.ptr; }
        inline bool operator==(SelfNoConstT k) const { return ptr == k.ptr; }
        inline bool operator!=(SelfNoConstT k) const { return ptr != k.ptr; }

        inline bool operator>(SelfConstT k) const { return ptr < k.ptr; }
        inline bool operator<(SelfConstT k) const { return ptr > k.ptr; }
        inline bool operator<=(SelfConstT k) const { return ptr >= k.ptr; }
        inline bool operator>=(SelfConstT k) const { return ptr <= k.ptr; }
        inline bool operator==(SelfConstT k) const { return ptr == k.ptr; }
        inline bool operator!=(SelfConstT k) const { return ptr != k.ptr; }

        inline size_t operator-(SelfConstT k) const { return k.ptr - ptr; }
        inline size_t operator-(SelfNoConstT k) const { return k.ptr - ptr; }

        inline BaseT base() const { return BaseT{ptr}; }

        inline T *operator->() const { return ptr; }

        inline T *Raw() { return ptr; }
        inline T *raw() { return ptr; }

        T *ptr;
    };

    

} // namespace ulib