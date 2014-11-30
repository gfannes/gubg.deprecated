#include "catch/catch.hpp"
#include "gubg/data/TableDistribution.hpp"
#include "gubg/data/Nominal.hpp"
#include "gubg/log/HR.hpp"
using namespace gubg::log;

namespace  { 
    typedef gubg::data::TableDistribution<3, float> TD;
    typedef gubg::data::Nominal<3> NV;
} 

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
TEST_CASE("ctor", "[tabledistr]")
{
    S();
    TD td;
    L(hr(td.probs()));
}
TEST_CASE("collecting nominal values", "[tabledistr]")
{
    S();
    TD td;
    td << NV(0);
    td << NV(1);
    td << NV(1);
    td << NV(1);
    td << NV(2);
    td << NV(2);
    L(hr(td.probs()));
}
TEST_CASE("Summing", "[tabledistr]")
{
    S();
    TD td1;
    {
        td1 << NV(0);
        td1 << NV(1);
        td1 << NV(1);
        td1 << NV(1);
        td1 << NV(2);
        td1 << NV(2);
    }
    TD td2;
    {
        td2 << NV(0);
        td2 << NV(0);
        td2 << NV(0);
        td2 << NV(1);
        td2 << NV(2);
        td2 << NV(2);
    }
    td1 += td2;
    L(hr(td1.probs()));
}
#include "gubg/log/end.hpp"
