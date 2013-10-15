#include "gubg/Testing.hpp"
#include "gubg/math/Operations.hpp"
using namespace gubg::math;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(math::Operations);
    unsigned int q, r;
    TEST_CODE(ReturnCode::OnlyUnsignedDivisionForNow, divide(q, r, -16, 4));
    TEST_CODE(ReturnCode::OnlyUnsignedDivisionForNow, divide(q, r, 16, -4));
    TEST_CODE(ReturnCode::DivisionByZero, divide(q, r, 16, 0));
    TEST_OK(divide(q, r, 16, 4));
    L("q: " << q << " r: " << r);
    TEST_EQ(4, q);
    TEST_EQ(0, r);
    return 0;
}
#include "gubg/log/end.hpp"
