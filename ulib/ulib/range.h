#pragma once

#include <cstddef>

namespace ulib
{
    class range_impl
    {
    public:
        class iterator
        {
        public:
            iterator(size_t c) noexcept : m(c) {}
            ~iterator() noexcept = default;
            size_t operator*() const noexcept { return m; }
            iterator &operator++() noexcept { return ++m, *this; }
            iterator &operator++(int) noexcept { return m++, *this; }
            bool operator==(iterator k) const noexcept { return m == k.m; }
            bool operator!=(iterator k) const noexcept { return m != k.m; }
        private:
            size_t m;
        };

        range_impl(size_t end) noexcept : ms(0), me(end) {}
        range_impl(size_t start, size_t end) noexcept : ms(start), me(end) {}
        ~range_impl() noexcept = default;

        iterator begin() const noexcept { return ms; }
        iterator end() const noexcept { return me; }

    private:
        size_t ms;
        size_t me;
    };

    inline range_impl range(size_t end) noexcept { return range_impl{end}; }
    inline range_impl range(size_t start, size_t end) noexcept { return range_impl{start, end}; }
} // namespace ulib