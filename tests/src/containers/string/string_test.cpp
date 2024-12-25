#include <type_traits>

#include <string>
#include <ulib/list.h>
#include <ulib/split.h>
#include <ulib/string.h>

#include <gtest/gtest.h>

TEST(StringTest, Constructs) { ASSERT_NO_THROW(ulib::String{"hi"}); }

TEST(StringTest, ConstructsWithPresetSize)
{
    ulib::String str(5);
    ASSERT_EQ(str.Size(), 5);
}

TEST(StringTest, ConstructsWithPresetCapacity)
{
    ulib::String str(ulib::args::Capacity{5});
    ASSERT_EQ(str.Capacity(), 5);
}

TEST(StringTest, ConstructsFromStdString)
{
    std::string source = "hello";
    ulib::String str = source;

    ASSERT_EQ(str, ulib::String(source.c_str()));
}

#include <typeinfo>

TEST(StringTest, ConstructsFromStringView)
{
    std::string_view source = "hello";
    ulib::String str = source;

    ASSERT_EQ(str, ulib::String(source.data(), source.size()));
}

TEST(StringTest, ConstructsFromUlibStringView)
{
    ulib::StringView source = "hello";
    ulib::String str = source;

    ASSERT_EQ(str, source);
}

TEST(StringTest, ConstructsFromPointerAndSize)
{
    char source[] = "hello world";
    ulib::String str{source, 5};

    ASSERT_EQ(str, "hello");
}

TEST(StringTest, ConstructsFromCharacterRange)
{
    char source[] = "hello world";
    ulib::String str{source, source + 5};

    ASSERT_EQ(str, "hello");
}

TEST(StringTest, EqualityComparison)
{
    ulib::String str = "foo";

    ASSERT_EQ(str, "foo");
    ASSERT_NE(str, "bar");
}

TEST(StringTest, CanAppend)
{
    ulib::String str = "hello";

    str.Append(" world");
    ASSERT_EQ(str, "hello world");
}

TEST(StringTest, CanAssign)
{
    ulib::String str = "hello";
    str = "goodbye";

    ASSERT_EQ(str, "goodbye");
}

TEST(StringTest, ConvertsToStringView)
{
    ulib::String str = "hello";
    std::string_view to = str;

    ASSERT_EQ(str, ulib::String(to.data(), to.size()));
}

TEST(StringTest, Pushing)
{
    ulib::String str = "hello ";
    str.PushBack('w');
    str.PushBack('o');
    str.PushBack('r');
    str.push_back('l');
    str.push_back('d');

    ASSERT_EQ(str, "hello world");
}

TEST(StringTest, Iteration)
{
    ulib::string str = "hello";
    ulib::string str2;
    for (auto ch : str)
    {
        str2.PushBack(ch);
    }

    ASSERT_EQ(str, str2);
}

TEST(StringTest, Addition)
{
    ulib::string str = "hello";
    ulib::string str2 = str + " world";

    ASSERT_EQ(str2, "hello world");

    str2 += " world";

    ASSERT_EQ(str2, "hello world world");
}

TEST(StringTest, Compares)
{
    auto test = [](const auto *kf, const auto *ks) {
        using EncodingT = ulib::literal_encoding_t<std::remove_cv_t<std::remove_reference_t<decltype(*kf)>>>;
        using StringViewT = ulib::EncodedStringView<EncodingT>;
        using StringT = ulib::EncodedString<EncodingT>;

        StringT f1 = kf;
        StringT f2 = kf;
        StringViewT f3 = kf;

        StringT s1 = ks;
        StringT s2 = ks;
        StringViewT s3 = ks;

        ASSERT_EQ(f1, f2);
        ASSERT_EQ(f2, f1);
        ASSERT_EQ(f1, kf);
        ASSERT_EQ(kf, f1);
        ASSERT_EQ(f1, f3);
        ASSERT_EQ(f3, f1);
        ASSERT_EQ(f3, kf);
        ASSERT_EQ(kf, f3);

        ASSERT_NE(f1, s1);
        ASSERT_NE(f1, ks);
        ASSERT_NE(s1, f1);
        ASSERT_NE(ks, f1);
        ASSERT_NE(f1, s3);
        ASSERT_NE(f3, s1);
        ASSERT_NE(f3, ks);
        ASSERT_NE(s3, f1);
        ASSERT_NE(s1, f3);
        ASSERT_NE(ks, f3);
    };

    test(u8"helloテ", u8"worldテ");
    test(u8"hello helloテ", u8"worldテ");
    test(u8"helloテ", u8"world worldテ");

    test(L"helloテ", L"worldテ");
    test(L"hello helloテ", L"worldテ");
    test(L"helloテ", L"world worldテ");

    test(u"helloテ", u"worldテ");
    test(u"hello helloテ", u"worldテ");
    test(u"helloテ", u"world worldテ");

    test(U"helloテ", U"worldテ");
    test(U"hello helloテ", U"worldテ");
    test(U"helloテ", U"world worldテ");

    test("hello", "world");
    test("hello hello", "world");
    test("hello", "world world");
}

TEST(StringTest, Copy)
{
    char8 kU8Str[] = u8"hello テ";

    ulib::u8string str1 = kU8Str;
    ulib::u8string str2 = str1;

    ASSERT_TRUE(str1 == kU8Str);
    ASSERT_TRUE(str2 == kU8Str);
    ASSERT_EQ(str1, str2);
    ASSERT_EQ(str2, str1);

    ulib::u8string str4;
    str4 = kU8Str;

    ASSERT_TRUE(str4 == kU8Str);
    ASSERT_EQ(str4, str1);
    ASSERT_EQ(str4, str2);

    ulib::u8string str5;
    str5 = str1;

    ASSERT_TRUE(str5 == kU8Str);
    ASSERT_EQ(str5, str1);
    ASSERT_EQ(str5, str2);

    ulib::u8string str6 = u8"jasdhfkasjh";
    str6 = kU8Str;

    ASSERT_TRUE(str6 == kU8Str);
    ASSERT_EQ(str6, str1);
    ASSERT_EQ(str6, str2);

    ulib::u8string str7 = u8"fdsgfdswdf";
    str7 = str1;

    ASSERT_TRUE(str7 == kU8Str);
    ASSERT_EQ(str7, str1);
    ASSERT_EQ(str7, str2);

    ulib::u8string str8 = u8"fdsgfdswdf";
    str8.Assign(str1);

    ASSERT_TRUE(str8 == kU8Str);
    ASSERT_EQ(str8, str1);
    ASSERT_EQ(str8, str2);

    ulib::u8string str9 = u8"fdsgfdswdf";
    str9.Assign(kU8Str);

    ASSERT_TRUE(str9 == kU8Str);
    ASSERT_EQ(str9, str1);
    ASSERT_EQ(str9, str2);
}

TEST(StringTest, CopyFromStringView)
{
    ulib::string_view vstr = "hello";

    ulib::string str2 = vstr;
    ASSERT_EQ(str2, vstr);

    str2 = vstr;
    ASSERT_EQ(str2, vstr);
}

TEST(StringTest, AdditionWithStringView)
{
    ulib::string str2;
    ulib::string_view vstr = "hello";

    str2 += vstr;

    ASSERT_EQ(str2, vstr);

    str2 = vstr;
    ASSERT_EQ(str2, vstr);
}

TEST(StringTest, StringViewSum)
{
    ulib::string_view vstr = "hello";
    ulib::string_view vstr2 = " world";

    ulib::string str = vstr + vstr2;

    ASSERT_EQ(str, "hello world");
}

TEST(StringTest, Assignment)
{
    ulib::string_view vstr = "hello";
    ulib::wstring_view vwstr = L"hello";
    ulib::u8string_view vu8str = u8"hello";
    ulib::u16string_view vu16str = u"hello";
    ulib::u32string_view vu32str = U"hello";

    ulib::string str = vstr;
    ulib::wstring wstr = vwstr;
    ulib::u8string u8str = vu8str;
    ulib::u16string u16str = vu16str;
    ulib::u32string u32str = vu32str;

    ASSERT_EQ(str, "hello");
    ASSERT_EQ(wstr, L"hello");
    ASSERT_EQ(u8str, u8"hello");
    ASSERT_EQ(u16str, u"hello");
    ASSERT_EQ(u32str, U"hello");

    str = vstr;
    wstr = vwstr;
    u8str = vu8str;
    u16str = vu16str;
    u32str = vu32str;

    ASSERT_EQ(str, "hello");
    ASSERT_EQ(wstr, L"hello");
    ASSERT_EQ(u8str, u8"hello");
    ASSERT_EQ(u16str, u"hello");
    ASSERT_EQ(u32str, U"hello");

    vstr = str;
    vwstr = wstr;
    vu8str = u8str;
    vu16str = u16str;
    vu32str = u32str;

    ASSERT_EQ(str, "hello");
    ASSERT_EQ(wstr, L"hello");
    ASSERT_EQ(u8str, u8"hello");
    ASSERT_EQ(u16str, u"hello");
    ASSERT_EQ(u32str, U"hello");

    ulib::string_view vstr1 = str;
    ulib::wstring_view vwstr1 = wstr;
    ulib::u8string_view vu8str1 = u8str;
    ulib::u16string_view vu16str1 = u16str;
    ulib::u32string_view vu32str1 = u32str;

    ASSERT_EQ(vstr1, "hello");
    ASSERT_EQ(vwstr1, L"hello");
    ASSERT_EQ(vu8str1, u8"hello");
    ASSERT_EQ(vu16str1, u"hello");
    ASSERT_EQ(vu32str1, U"hello");

    vstr1 = vstr;
    vwstr1 = vwstr;
    vu8str1 = vu8str;
    vu16str1 = vu16str;
    vu32str1 = vu32str;

    ASSERT_EQ(vstr1, "hello");
    ASSERT_EQ(vwstr1, L"hello");
    ASSERT_EQ(vu8str1, u8"hello");
    ASSERT_EQ(vu16str1, u"hello");
    ASSERT_EQ(vu32str1, U"hello");

    ulib::string_view vstr2 = vstr;
    ulib::wstring_view vwstr2 = vwstr;
    ulib::u8string_view vu8str2 = vu8str;
    ulib::u16string_view vu16str2 = vu16str;
    ulib::u32string_view vu32str2 = vu32str;

    ASSERT_EQ(vstr2, "hello");
    ASSERT_EQ(vwstr2, L"hello");
    ASSERT_EQ(vu8str2, u8"hello");
    ASSERT_EQ(vu16str2, u"hello");
    ASSERT_EQ(vu32str2, U"hello");
}

TEST(StringTest, Compare)
{
    ulib::string_view vstr = "hello";
    ulib::wstring_view vwstr = L"hello";
    ulib::u8string_view vu8str = u8"hello";
    ulib::u16string_view vu16str = u"hello";
    ulib::u32string_view vu32str = U"hello";

    ulib::string str = vstr;
    ulib::wstring wstr = vwstr;
    ulib::u8string u8str = vu8str;
    ulib::u16string u16str = vu16str;
    ulib::u32string u32str = vu32str;

    ASSERT_EQ(str == "hello", true);
    ASSERT_EQ(wstr == L"hello", true);
    ASSERT_EQ(u8str == u8"hello", true);
    ASSERT_EQ(u16str == u"hello", true);
    ASSERT_EQ(u32str == U"hello", true);

    ASSERT_EQ(vstr == "hello", true);
    ASSERT_EQ(vwstr == L"hello", true);
    ASSERT_EQ(vu8str == u8"hello", true);
    ASSERT_EQ(vu16str == u"hello", true);
    ASSERT_EQ(vu32str == U"hello", true);

    ASSERT_EQ(str == vstr, true);
    ASSERT_EQ(wstr == vwstr, true);
    ASSERT_EQ(u8str == vu8str, true);
    ASSERT_EQ(u16str == vu16str, true);
    ASSERT_EQ(u32str == vu32str, true);

    ASSERT_EQ(vstr == str, true);
    ASSERT_EQ(vwstr == wstr, true);
    ASSERT_EQ(vu8str == u8str, true);
    ASSERT_EQ(vu16str == u16str, true);
    ASSERT_EQ(vu32str == u32str, true);

    ASSERT_EQ(vstr == vstr, true);
    ASSERT_EQ(vwstr == vwstr, true);
    ASSERT_EQ(vu8str == vu8str, true);
    ASSERT_EQ(vu16str == vu16str, true);
    ASSERT_EQ(vu32str == vu32str, true);

    ASSERT_EQ(str == str, true);
    ASSERT_EQ(wstr == wstr, true);
    ASSERT_EQ(u8str == u8str, true);
    ASSERT_EQ(u16str == u16str, true);
    ASSERT_EQ(u32str == u32str, true);
}

TEST(StringTest, Length)
{
    char kStr[] = "fullest ky";
    wchar_t kWstr[] = L"fullest ky";
    char8 kU8str[] = u8"полный ку";
    char16 kU16str[] = u"полный ку";
    char32 kU32str[] = U"полный ку";

    ulib::string str = kStr;
    ulib::wstring wstr = kWstr;
    ulib::u8string u8str = kU8str;
    ulib::u16string u16str = kU16str;
    ulib::u32string u32str = kU32str;

#define arrsize(x) (sizeof(x) / sizeof(x[0]))
#define cstrsize(x) (arrsize(x) - 1)

    EXPECT_EQ(str.size(), cstrsize(kStr));
    EXPECT_EQ(wstr.size(), cstrsize(kWstr));
    EXPECT_EQ(u8str.size(), cstrsize(kU8str));
    EXPECT_EQ(u16str.size(), cstrsize(kU16str));
    EXPECT_EQ(u32str.size(), cstrsize(kU32str));

    EXPECT_EQ(str.length(), 10);
    EXPECT_EQ(wstr.length(), 10);
    EXPECT_EQ(u8str.length(), 9);
    EXPECT_EQ(u16str.length(), 9);
    EXPECT_EQ(u32str.length(), 9);

#undef cstrsize
#undef arrsize
}

TEST(StringTest, Resize)
{
    ulib::u8string u8str;
    u8str.resize(20);
    ASSERT_EQ(u8str.size(), 20);

    ulib::u16string u16str;
    u16str.resize(20);
    ASSERT_EQ(u16str.size(), 20);

    u8str.resize(10);
    u16str.resize(10);

    ASSERT_EQ(u8str.size(), 10);
    ASSERT_EQ(u16str.size(), 10);

    u8str.resize(0);
    u16str.resize(0);

    ASSERT_EQ(u8str.size(), 0);
    ASSERT_EQ(u16str.size(), 0);

    u8str.resize(15, u8'0');
    u16str.resize(15, u'1');

    ASSERT_EQ(u8str.size(), 15);
    ASSERT_EQ(u16str.size(), 15);

    ASSERT_EQ(u8str, u8"000000000000000");
    ASSERT_EQ(u16str, u"111111111111111");
}

TEST(StringTest, Reserve)
{
    ulib::u8string u8str;
    ulib::u16string u16str;

    // TODO: Concretize starting capacity

    auto test = [&](size_t size) {
        u8str.reserve(size);
        u16str.reserve(size);

        ASSERT_TRUE(u8str.capacity() >= size);
        ASSERT_TRUE(u16str.capacity() >= size);
    };

    test(30);
    test(4000);
    test(10);
}

TEST(StringTest, FrontAndBack)
{
    ulib::string str = "ABCDEF";

    ASSERT_EQ(str.front(), 'A');
    ASSERT_EQ(str.back(), 'F');
}

TEST(StringTest, At)
{
    ulib::string str = "ABCDEF";

    ASSERT_EQ(str.at(0), 'A');
    ASSERT_EQ(str.at(5), 'F');
    ASSERT_EQ(str.at(4), 'E');

    ASSERT_THROW(str.at(6), std::out_of_range);
}

TEST(StringTest, ConstructFromSpan)
{
    {
        char arr[] = {'a', 'b', 'c'};
        ulib::span<char> spn = arr;
        ulib::string str = spn;

        ASSERT_EQ(str, spn);
    }

    {
        char8 arr[] = {'a', 'b', 'c'};
        ulib::span<char8> spn = arr;
        ulib::u8string str = spn;

        ASSERT_EQ(str, spn);
    }
}

#include <ulib/range.h>

TEST(StringTest, ParentEncoding)
{
    for (auto i : ulib::range(40))
    {
        ulib::u8string str = u8"hello";
        str.resize(i + 5);

        ulib::string_view vstr = str;
        ulib::string str0 = vstr;

        ASSERT_EQ(str.raw(), vstr.raw());
        ASSERT_EQ(str0, vstr);
    }
}

TEST(StringTest, CopyConstruct)
{
    ulib::u8string str = u8"asdfkjdsaf";
    ulib::u8string str1 = str;

    ASSERT_NE(str.data(), str1.data());
}

TEST(StringTest, ConstructFromCompatible) { ulib::string str = ulib::u8string(u8"asdfkjdsaf"); }

TEST(StringTest, Replace)
{
    ulib::string str{"12plak28plak256fullplak"};
    ulib::string result = str.replace("plak", "ky");

    ASSERT_EQ(result, "12ky28ky256fullky");

    result = result.replace("ky", "");
    ASSERT_EQ(result, "1228256full");

    result = result.replace("full", "555555");
    ASSERT_EQ(result, "1228256555555");

    result = result.replace("1228256", "");
    ASSERT_EQ(result, "555555");

    result = result.replace("5", "");
    ASSERT_EQ(result, "");
}

TEST(StringTest, RemoveAll)
{
    ulib::string str{"1plak2plak3plak4"};
    ulib::string result = str.remove_all("plak");

    ASSERT_EQ(result, "1234");
}

TEST(StringTest, LiteralAddition)
{
    {
        ulib::string str{"full"};
        ulib::string result = "plak " + str;

        ASSERT_EQ(result, "plak full");
    }

    {
        ulib::u8string str{u8"full"};
        ulib::u8string result = u8"plak " + str;

        ASSERT_EQ(result, u8"plak full");
    }

    {
        ulib::string_view str{"full"};
        ulib::string result = "plak " + str;

        ASSERT_EQ(result, "plak full");
    }

    {
        ulib::u8string_view str{u8"full"};
        ulib::u8string result = u8"plak " + str;

        ASSERT_EQ(result, u8"plak full");
    }
}

TEST(StringTest, Mul)
{
    auto mul1 = ulib::string{"test"} * 3;
    auto mul2 = ulib::string_view{"test"} * 3;

    ASSERT_EQ(mul1, mul2);
    ASSERT_EQ(mul1, "testtesttest");
    ASSERT_EQ(mul2, "testtesttest");
}


TEST(StringTest, LowerBigger)
{
    ulib::string abc = "abc";
    ulib::string def = "def";

    ASSERT_TRUE(abc < def);
    ASSERT_TRUE(def > abc);
}