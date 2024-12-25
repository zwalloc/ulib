#include <gtest/gtest.h>

#include <ulib/string.h>

TEST(StringViewTest, Empty)
{
    ulib::string_view view;
    ASSERT_TRUE(view.empty());

    view = "hello";
    ASSERT_FALSE(view.empty());
}

TEST(StringViewTest, Size)
{
    ulib::string_view view;
    ASSERT_EQ(view.size(), 0);

    view = "hello";
    ASSERT_EQ(view.size(), 5);
}

TEST(StringViewTest, Contains)
{
    ulib::string_view view;
    ASSERT_FALSE(view.contains("vi"));

    view = "view";
    ASSERT_TRUE(view.contains("vi"));
    ASSERT_TRUE(view.starts_with("vi"));
    ASSERT_TRUE(!view.ends_with("vi"));
    ASSERT_TRUE(view.ends_with("ew"));
    ASSERT_TRUE(view.contains("ew"));
}

TEST(StringViewTest, Reverse)
{
    ulib::string_view view;

    view = "view";
    char rr[] = "weiv";

    size_t idx = 0;
    for (auto ch : view.reverse())
    {
        ASSERT_EQ(ch, rr[idx]);
        idx++;
    }
}

TEST(StringViewTest, Length)
{
    ulib::u8string_view view = u8"три";
    ASSERT_EQ(view.length(), 3);
    ASSERT_EQ(view.size(), 6);
}

TEST(StringViewTest, RemoveSuffixAndPrefix)
{
    ulib::string_view view = "hello";

    view.remove_suffix(1);
    ASSERT_EQ(view, "hell");

    view.remove_prefix(1);
    ASSERT_EQ(view, "ell");
}

TEST(StringViewTest, ConstructFromSpan)
{
    char buf[] = {'h', 'e', 'l', 'l', 'o'};
    ulib::span<char> span = buf;
    ulib::string_view view = span;

    ASSERT_EQ(view, "hello");
    ASSERT_EQ(view, span);
}

TEST(StringViewTest, strip)
{
    ulib::string_view text0 = "111text111";
    ulib::string_view text1 = "1text1";
    ulib::string_view text2 = "text1";
    ulib::string_view text3 = "21text12";
    ulib::string_view text4 = "21text1";

    ASSERT_EQ(text0.strip("1"), "text");
    ASSERT_EQ(text1.strip("1"), "text");
    ASSERT_EQ(text2.strip("1"), "text");
    ASSERT_EQ(text3.strip("1"), "21text12");
    ASSERT_EQ(text4.strip("1"), "21text");

    ASSERT_EQ(text3.strip("12"), "text");
    ASSERT_EQ(text4.strip("21"), "text");
}

TEST(StringViewTest, lstrip)
{
    ulib::string_view text0 = "111text111";
    ulib::string_view text1 = "1text1";
    ulib::string_view text2 = "text1";
    ulib::string_view text3 = "21text12";
    ulib::string_view text4 = "21text1";

    ASSERT_EQ(text0.lstrip("1"), "text111");
    ASSERT_EQ(text1.lstrip("1"), "text1");
    ASSERT_EQ(text2.lstrip("1"), "text1");
    ASSERT_EQ(text3.lstrip("1"), "21text12");
    ASSERT_EQ(text4.lstrip("1"), "21text1");

    ASSERT_EQ(text3.lstrip("12"), "text12");
    ASSERT_EQ(text4.lstrip("21"), "text1");
}

TEST(StringViewTest, rstrip)
{
    ulib::string_view text0 = "111text111";
    ulib::string_view text1 = "1text1";
    ulib::string_view text2 = "text1";
    ulib::string_view text3 = "21text12";
    ulib::string_view text4 = "21text1";

    ASSERT_EQ(text0.rstrip("1"), "111text");
    ASSERT_EQ(text1.rstrip("1"), "1text");
    ASSERT_EQ(text2.rstrip("1"), "text");
    ASSERT_EQ(text3.rstrip("1"), "21text12");
    ASSERT_EQ(text4.rstrip("1"), "21text");

    ASSERT_EQ(text3.rstrip("12"), "21text");
    ASSERT_EQ(text4.rstrip("21"), "21text");
}

TEST(StringViewTest, starts_with)
{
    ulib::string_view text0 = "text";
    
    ASSERT_TRUE(text0.starts_with("tex"));
    ASSERT_TRUE(text0.starts_with("t"));
    ASSERT_TRUE(text0.starts_with('t'));
    ASSERT_FALSE(text0.starts_with('e'));

    text0.remove_suffix(text0.length());

    ASSERT_FALSE(text0.starts_with("t"));
    ASSERT_FALSE(text0.starts_with('t'));

    ASSERT_TRUE(text0.starts_with(""));
}

TEST(StringViewTest, ends_with)
{
    ulib::string_view text0 = "text";
    
    ASSERT_TRUE(text0.ends_with("ext"));
    ASSERT_TRUE(text0.ends_with("t"));
    ASSERT_TRUE(text0.ends_with('t'));
    ASSERT_FALSE(text0.ends_with('x'));

    text0.remove_suffix(text0.length());

    ASSERT_TRUE(text0.ends_with(""));
}