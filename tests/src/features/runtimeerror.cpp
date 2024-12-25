#include <gtest/gtest.h>

#include <ulib/runtimeerror.h>

TEST(RuntimeError, Throw)
{
    EXPECT_THROW({ throw ulib::RuntimeError{}; }, ulib::RuntimeError);
    EXPECT_THROW({ throw ulib::RuntimeError{}; }, std::runtime_error);
}

TEST(RuntimeError, WithText)
{
    try
    {
        throw ulib::RuntimeError{"hello"};
    }
    catch(const ulib::RuntimeError& error)
    {
        EXPECT_EQ(ulib::string_view(error.what()), "hello");
    }
    catch(...)
    {
        ASSERT_TRUE(false);
    }
}