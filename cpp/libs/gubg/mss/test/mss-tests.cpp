#include "gubg/mss.hpp"
#include <iostream>

enum class Compare {MSS_DEFAULT_CODES, Smaller, Larger};
MSS_CODE_BEGIN(Compare);
MSS_CODE_(Debug, Smaller);
MSS_CODE_(Debug, Larger);
MSS_CODE_END();
Compare compare(int lhs, int rhs)
{
    MSS_BEGIN(Compare);
    MSS_T(lhs <= rhs, Larger);
    MSS_T(lhs >= rhs, Smaller);
    MSS_END();
}

enum class ReturnCode
{
    OK, Error, False, NotSoSerious, Serious
};

ReturnCode frc()
{
    MSS_BEGIN(ReturnCode);
    MSS(ReturnCode::Error);
    MSS_END();
}

void fv()
{
    MSS_BEGIN(void);
    MSS(ReturnCode::OK);
    MSS(frc());
    MSS_END();
}

void f()
{
    MSS_BEGIN(void);
    MSS(false);
    MSS_END();
}

void f_log()
{
    MSS_BEGIN_(void, "f_log");
    return;
    MSS_END_();
}
void f_log2()
{
    MSS_BEGIN_(void, "f_log");
    MSS(false);
    MSS_END_();
}
void f_log3()
{
    try
    {
        MSS_BEGIN_(void, "f_log");
        throw "LetsJumpAway";
        MSS_END_();
    }
    catch (...){}
}
Compare notImplemented()
{
    MSS_BEGIN_(Compare, "notImplemented");
    MSS_L(NotImplemented);
    MSS_END_();
}

ReturnCode allowed()
{
    MSS_BEGIN(ReturnCode, AllowOtherCodes);
    MSS_ALLOW(NotSoSerious);
    MSS_L(NotSoSerious);
    MSS_L(Serious);
    MSS_END();
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
    frc();
    fv();
    f();

    f_log();
    f_log2();
    f_log3();
    notImplemented();

    TEST_EQ_TYPE(int, ReturnCode::Serious, allowed());
    return 0;
}
