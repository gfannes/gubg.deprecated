#include "gubg/mss.hpp"
#include "gubg/sleep/sleep.hpp"

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
enum class Compare {MSS_DEFAULT_CODES, Smaller, Larger};
MSS_CODES_BEGIN(Compare);
MSS_CODE_(Debug, Smaller);
MSS_CODE_(Debug, Larger);
MSS_CODES_END();
Compare compare(int lhs, int rhs)
{
    MSS_BEGIN(Compare);
    MSS(lhs <= rhs, Larger);
    MSS(lhs >= rhs, Smaller);
    MSS_END();
}

enum class ReturnCode
{
    MSS_DEFAULT_CODES, NotSoSerious, Serious, NotFound, Error
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
    //Signalled as unsuccessful, we quit before MSS_END
    return;
    MSS_END_();
}
void f_log2()
{
    MSS_BEGIN_(void, "f_log2");
    //Signalled as unsuccessful
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

ReturnCode findSomething(){return ReturnCode::NotFound;}
ReturnCode findSomething2(){return ReturnCode::Error;}
ReturnCode skip_if()
{
    MSS_BEGIN(ReturnCode);
    MSS_SKIP_IF(findSomething(), NotFound)
    {
        L("I found it");
    }
    //Check that we are still following the MSS
    MSS(MSS_RC_VAR.v_);
    L("Could not find it");
    MSS_SKIP_IF(findSomething2(), NotFound)
    {
        L("Found it 2");
    }
    L("Could not find the second either");
    MSS_SKIP_IF(compare(1, 2), Larger)
    {
    }
    MSS_END();
}
ReturnCode do_if()
{
    MSS_BEGIN(ReturnCode);
    MSS_DO_IF(findSomething(), NotFound)
    {
        L("Keep looking");
    }
    L("MSS continues");
    MSS_END();
}

ReturnCode time()
{
    MSS_BEGIN_PROFILE(ReturnCode, "blabla");
    gubg::nanosleep(2, 5000000);
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

    skip_if();
    do_if();

    time();
    return 0;
}
#include "gubg/log/end.hpp"
