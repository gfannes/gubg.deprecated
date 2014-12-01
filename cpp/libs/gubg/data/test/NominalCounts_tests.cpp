#include "catch/catch.hpp"
#include "gubg/data/NominalCounts.hpp"
#include "gubg/data/Nominal.hpp"
#include "gubg/log/HR.hpp"
using namespace gubg::log;

namespace  { 
    typedef gubg::data::NominalCounts<3, float> NC;
    typedef gubg::data::Nominal<3> NV;
} 

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
TEST_CASE("ctor", "[tabledistr]")
{
    S();
    NC nc;
    L(hr(nc.counts()));
}
TEST_CASE("collecting nominal values", "[tabledistr]")
{
    S();
    NC nc;
    nc << NV(0);
    nc << NV(1);
    nc << NV(1);
    nc << NV(1);
    nc << NV(2);
    nc << NV(2);
    L(hr(nc.counts()));
}
TEST_CASE("Summing", "[tabledistr]")
{
    S();
    NC nc1;
    {
        nc1 << NV(0);
        nc1 << NV(1);
        nc1 << NV(1);
        nc1 << NV(1);
        nc1 << NV(2);
        nc1 << NV(2);
    }
    NC nc2;
    {
        nc2 << NV(0);
        nc2 << NV(0);
        nc2 << NV(0);
        nc2 << NV(1);
        nc2 << NV(2);
        nc2 << NV(2);
    }
    nc1 += nc2;
    L(hr(nc1.counts()));
    REQUIRE(12 == nc1.sum());
}
#include "gubg/log/end.hpp"
