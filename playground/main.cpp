
#include <iostream>
#include <ulib/chrono.h>
#include <ulib/encodings/w1251/string.h>
#include <ulib/string.h>
#include <ulib/strutility.h>

#include <ulib/containers/filterview.h>
#include <ulib/containers/mapview.h>

namespace ulib
{
    namespace detail
    {
        template <typename Sig, typename C>
        inline Sig C::*overload_resolve_v(std::false_type, Sig C::*mem_func_ptr)
        {
            return mem_func_ptr;
        }

        template <typename Sig, typename C>
        inline Sig C::*overload_resolve_v(std::true_type, Sig C::*mem_variable_ptr)
        {
            return mem_variable_ptr;
        }
    } // namespace detail

    template <typename Sig, typename C>
    inline Sig C::*overload(Sig C::*mem_ptr)
    {
        return detail::overload_resolve_v(std::is_member_object_pointer<Sig C::*>(), mem_ptr);
    }
} // namespace ulib

template <class AllocatorT>
class LoggedAllocator : public AllocatorT
{
public:
    LoggedAllocator(typename AllocatorT::Params params = {}) : AllocatorT(params) {}

    inline void *Alloc(size_t size)
    {
        auto result = AllocatorT::Alloc(size);
        printf(" [ALLOC] Allocating %llu bytes\n", size);
        return result;
    }

    inline void Free(void *ptr)
    {
        AllocatorT::Free(ptr);
        // printf("");
    }
};

#define ULIB_ANY_CALL(fn) ([](auto &&...args) { return (fn)(std::forward<decltype(args)>(args)...); })
#define ULIB_MEM_CALL(fn) ([](auto &instance, auto &&...args) { return ((instance).*(fn))(args...); })

void test()
{
    ulib::string str = "abcdefghijklmnop123456789ABCDEFGH~!";
    auto groupped = str.group_by([](char ch) -> ulib::string {
        if (std::isdigit(ch))
            return "digits";
        else if (std::isupper(ch))
            return "upper-case chars";
        else if (std::islower(ch))
            return "lower-case chars";
        else
            return "something else";
    });

    for (auto &group : groupped)
    {
        printf("tag: %s\n", group.first.c_str());
        for (auto &item : group.second)
            printf(" - %c\n", item);
    }

    ulib::list<ulib::string> strs = {"abc", "abd", "cde"};

    for (auto &group : strs.group_by([](const ulib::string &str) -> ulib::string { return str.substr(0, 1); }))
    {
        printf("tag: %s\n", group.first.c_str());
        for (auto &item : group.second)
            printf(" - %s\n", item.c_str());
    }

    {
        ulib::List<std::pair<const char *, ulib::List<char>>> groupped = ulib::group_by("abcdefghijklmnop123456789ABCDEFGH~!", [](char ch) {
            if (std::isdigit(ch))
                return "digits";
            else if (std::isupper(ch))
                return "upper-case chars";
            else if (std::islower(ch))
                return "lower-case chars";
            else
                return "something else";
        });

        for (auto &group : groupped)
        {
            printf("tag: %s\n", group.first);
            printf(" - %s\n", ulib::string{group.second}.c_str());
        }
    }
}

int main()
{
    test();
    return 0;

    /*
    // int (TMyClass::*pt2ConstMember)(float, char, char) const

    // ulib::string_view text0 = "111text111";
    // printf("%s\n", ulib::str(text0.strip("1")).c_str());

    // ulib::string str0 = "text";

    ulib::list<ulib::string> strs = {"one", "two", "thr ee", "things", "foo bar"};

    auto predicate = [](const ulib::string &str, const ulib::string &sep) {
        std::printf("[Invoking predicate on '%s']\n", str.c_str());
        return str + sep + str;
    };

    printf("Lazy-mapped:\n");

    auto strs_view = strs.ToSpan();

    auto view = strs_view.map(predicate, ".").map(predicate, "_"); // ulib::MapView{ulib::MapView{strs, predicate, "."}, predicate, "_"};

    for (auto s : view)
    {
        printf("%s\n", ulib::string{s}.data());
    }

    printf("\n");

    strs.transform(&ulib::string::push_back, '?');

    for (auto strs : strs.map(&ulib::string::split, " "))
    {
        for (auto v : strs)
            printf("v: %s\n", ulib::string{v}.c_str());
    }

    for (auto size : strs.map(&ulib::string::size))
    {
        printf("size: %llu\n", size);
    }

    auto pred = [](const ulib::string &str) { return str.starts_with("tw"); };

    strs.filter(pred).transform(&ulib::string::push_back, '!');
    strs.filter(pred).transform(&ulib::string::push_back, '!');
    strs.filter(pred).transform(&ulib::string::push_back, '!');

    for (auto b : strs.filter(pred).map(&ulib::string::c_str))
    {
        printf("starts_with: %s\n", b);
    }

    printf("\n\n\nALLOC TEST\n\n\n");

    using LoggedDefaultAlloc = LoggedAllocator<ulib::DefaultAllocator>;
    using LoggedList = ulib::List<ulib::string, LoggedDefaultAlloc>;

    LoggedList testList{"FullPlak", "TimTimson", "JasonWilson", "Test"};

    printf("Normal filter/map:\n");

    LoggedList normalFilterMap = testList.filter([](const ulib::string &s) { return !s.starts_with("Tim"); })
                                     .map(&ulib::string::replace, "a", "@")
                                     .map(&ulib::string::replace, "l", "bb")
                                     .filter([](const ulib::string &s) { return !s.starts_with("Tim"); });

    for (auto s : normalFilterMap)
    {
        printf("s: %s\n", s.c_str());
    }

    printf("Lazy filter/map:\n");
    ulib::List lazyFilterMap = testList.filter([](const ulib::string &s) { return !s.starts_with("Tim"); })
                                   .map(&ulib::string::replace, "a", "@")
                                   .map(&ulib::string::replace, "l", "bb")
                                   .map(&ulib::string::size)
                                   // .filter(ULIB_MEM_CALL(&ulib::string::starts_with), "test")
                                   .filter(std::greater<int>{}, 5)
                                   .map(ULIB_ANY_CALL(std::to_string));

    for (auto s : lazyFilterMap)
    {
        printf("s: %s\n", s.c_str());
    }
    */

    // ulib::string str1(str0.begin(), str0.end());

    // ulib::list<int> vals = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    // constexpr int ky = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 11 + 12 + 13;

    // auto filterVals = vals.filter([](auto v) { return v <= 8; });
    // auto reduceVals = vals.reduce([](auto left, auto right) { return left + right; });
    // auto mapVals = vals.map([](auto v) { return v + 10; });

    // auto printList = [](ulib::list<int> &list) {
    //     ulib::string result;
    //     for (auto obj : list)
    //     {
    //         result.append(std::to_string(obj) + "-");
    //     }

    //     return result;
    // };

    // printf("filterVals: %s\n", printList(filterVals).c_str());
    // printf("reduceVals: %d\n", reduceVals);
    // printf("mapVals: %s\n", printList(mapVals).c_str());

    return 0;
}