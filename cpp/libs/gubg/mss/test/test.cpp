#include "gubg/mss.hpp"
#include <iostream>

enum class Compare {MSS_DEFAULT_CODES, Smaller, Larger};
MSS_CODE_BEGIN(Compare);
MSS_CODE_(Info, Smaller);
MSS_CODE(Larger);
MSS_CODE_END();

Compare compare(int lhs, int rhs)
{
    MSS_BEGIN(Compare);
    MSS_T(lhs >= 0, IllegalArgument);
    MSS_T(rhs >= 0, IllegalArgument);
    MSS_T(lhs >= rhs, Smaller);
    MSS_T_(Info, lhs <= rhs, Larger, lhs << " is larger than " << rhs);
    MSS_END();
}
Compare fasterThanLight()
{
    return Compare::NotImplemented;
}
Compare superCompare(int lhs, int rhs)
{
    MSS_BEGIN(Compare);
    MSS(fasterThanLight());
    MSS_END();
}

#define TEST_TAG(tag)
#define TEST_EQ_TYPE(t, e, a) if ((e) != (a)) std::cout << "Problem on line " << __LINE__ << std::endl
int main()
{
    TEST_TAG(mss_main);
    TEST_EQ_TYPE(int, Compare::OK, compare(0, 0));
    TEST_EQ_TYPE(int, Compare::OK, compare(1, 1));
    TEST_EQ_TYPE(int, Compare::IllegalArgument, compare(-2, -2));
    TEST_EQ_TYPE(int, Compare::Smaller, compare(0, 1));
    TEST_EQ_TYPE(int, Compare::Larger, compare(1, 0));
    superCompare(0, 0);
    //This should never be seen, fasterThanLight() is not implemented, which should cause an immediate halt
    TEST_EQ_TYPE(int, 0, 1);
    return 0;
}
