#include "gubg/mss.hpp"
#include <iostream>

enum class Compare {OK, Smaller, Larger};
Compare compare(int lhs, int rhs)
{
    MSS_BEGIN(Compare);
    MSS_T(lhs <= rhs, Larger);
    MSS_T(lhs >= rhs, Smaller);
    MSS_FAIL();
    MSS_RETURN();
}

#define TEST_TAG(tag)
#define TEST_EQ_TYPE(t, e, a) if ((e) != (a)) std::cout << "Problem " << __LINE__ << std::endl
int main()
{
    TEST_TAG(mss_main);
    TEST_EQ_TYPE(int, Compare::OK, compare(0, 0));
    TEST_EQ_TYPE(int, Compare::OK, compare(1, 1));
    TEST_EQ_TYPE(int, Compare::OK, compare(-2, -2));
    TEST_EQ_TYPE(int, Compare::Smaller, compare(0, 1));
    TEST_EQ_TYPE(int, Compare::Larger, compare(1, 0));
    return 0;
}
