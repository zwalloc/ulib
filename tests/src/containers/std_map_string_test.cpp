#include <map>
#include <ulib/string.h>

#include <gtest/gtest.h>

TEST(StdMapStringTest, Constructs)
{
#ifdef __cpp_char8_t
    using std_u8string = std::u8string;
#else
    using std_u8string = std::string;
#endif

    std::map<ulib::string, ulib::string> map1;
    std::map<ulib::wstring, ulib::wstring> map2;
    std::map<ulib::u8string, ulib::u8string> map3;
    std::map<ulib::u16string, ulib::u16string> map4;
    std::map<ulib::u32string, ulib::u32string> map5;

    std::map<ulib::string, std::string> map11;
    std::map<ulib::wstring, std::wstring> map12;
    std::map<ulib::u8string, std_u8string> map13;
    std::map<ulib::u16string, std::u16string> map14;
    std::map<ulib::u32string, std::u32string> map15;

    std::map<ulib::string, ulib::string> map21;
    std::map<ulib::wstring, ulib::u8string> map22;
    std::map<ulib::u8string, std::u16string> map23;
    std::map<ulib::u16string, ulib::string> map24;
    std::map<ulib::u32string, ulib::u8string> map25;

    std::map<std::string, ulib::string> map31;
    std::map<std::wstring, ulib::wstring> map32;
    std::map<std_u8string, ulib::u8string> map33;
    std::map<std::u16string, ulib::u16string> map34;
    std::map<std::u32string, ulib::u32string> map35;

    std::map<std::string, ulib::string> map41;
    std::map<std::wstring, ulib::u8string> map42;
    std::map<std_u8string, std::u16string> map43;
    std::map<std::u16string, ulib::string> map44;
    std::map<std::u32string, ulib::u8string> map45;
}

TEST(StdMapStringTest, Store)
{
    std::map<ulib::string, ulib::string> map1;
    std::map<ulib::wstring, ulib::wstring> map2;
    std::map<ulib::u8string, ulib::u8string> map3;
    std::map<ulib::u16string, ulib::u16string> map4;
    std::map<ulib::u32string, ulib::u32string> map5;

    map1["plak"] = "full";
    map2[L"plak"] = L"full";
    map3[u8"plak"] = u8"full";
    map4[u"plak"] = u"full";
    map5[U"plak"] = U"full";

    ASSERT_EQ(map1["plak"], "full");
    ASSERT_EQ(map2[L"plak"], L"full");
    ASSERT_EQ(map3[u8"plak"], u8"full");
    ASSERT_EQ(map4[u"plak"], u"full");
    ASSERT_EQ(map5[U"plak"], U"full");

    map1["k3"] = "re6";
    map2[L"k3"] = L"re6";
    map3[u8"k3"] = u8"re6";
    map4[u"k3"] = u"re6";
    map5[U"k3"] = U"re6";

    ASSERT_EQ(map1["k3"], "re6");
    ASSERT_EQ(map2[L"k3"], L"re6");
    ASSERT_EQ(map3[u8"k3"], u8"re6");
    ASSERT_EQ(map4[u"k3"], u"re6");
    ASSERT_EQ(map5[U"k3"], U"re6");
}

TEST(StdMapStringTest, MultipleSelection)
{
    std::map<ulib::u8string, int> map;

    map[u8"one"] = 0;
    map[u8"two"] = 1;
    map[u8"three"] = 2;
    map[u8"four"] = 3;
    map[u8"five"] = 4;
    map[u8"six"] = 5;
    map[u8"seven"] = 6;
    map[u8"eight"] = 7;
    map[u8"nine"] = 8;
    map[u8"ten"] = 9;

    ASSERT_EQ(map.size(), 10);
    ASSERT_EQ(map[u8"one"], 0);
    ASSERT_EQ(map[u8"two"], 1);
    ASSERT_EQ(map[u8"three"], 2);
    ASSERT_EQ(map[u8"four"], 3);
    ASSERT_EQ(map[u8"five"], 4);
    ASSERT_EQ(map[u8"six"], 5);
    ASSERT_EQ(map[u8"seven"], 6);
    ASSERT_EQ(map[u8"eight"], 7);
    ASSERT_EQ(map[u8"nine"], 8);
    ASSERT_EQ(map[u8"ten"], 9);

    map[u8"far"] = 10;
    map[u8"farfar"] = 11;
    map[u8"freeze"] = 12;
    map[u8"unfreeze"] = 13;
    map[u8"reclear"] = 14;
    map[u8"clear"] = 15;
    map[u8"freezes"] = 16;
    map[u8"freezefarfarclear"] = 17;

    ASSERT_EQ(map[u8"far"], 10);
    ASSERT_EQ(map[u8"farfar"], 11);
    ASSERT_EQ(map[u8"freeze"], 12);
    ASSERT_EQ(map[u8"unfreeze"], 13);
    ASSERT_EQ(map[u8"reclear"], 14);
    ASSERT_EQ(map[u8"clear"], 15);
    ASSERT_EQ(map[u8"freezes"], 16);
    ASSERT_EQ(map[u8"freezefarfarclear"], 17);
}