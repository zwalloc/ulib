#include <gtest/gtest.h>

#include <ulib/span.h>
#include <ulib/string.h>
#include <ulib/list.h>

TEST(SpanWithStringTest, ContainsAndFind)
{
    ulib::string strs[] = { "hello", "world", "alpha", "core", "plak", "mp" };
    ulib::span<ulib::string> spn = strs;

    ASSERT_TRUE(spn.contains("world"));
    ASSERT_TRUE(spn.contains(spn.subspan(1, 3)));
    ASSERT_TRUE(spn.contains(ulib::list<ulib::string>{"plak", "mp"}));

    ASSERT_TRUE(spn.starts_with(ulib::list<ulib::string>{"hello", "world"}));
    ASSERT_TRUE(spn.ends_with(ulib::list<ulib::string>{"plak", "mp"}));

    ASSERT_TRUE(spn.find(ulib::list<ulib::string>{"alpha", "core"}) == 2);
    ASSERT_TRUE(spn.rfind(ulib::list<ulib::string>{"alpha", "core"}) == 2);

    ASSERT_TRUE(spn.find("plak") == 4);
    ASSERT_TRUE(spn.rfind("plak") == 4);
}
