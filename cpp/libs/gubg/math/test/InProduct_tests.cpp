#include "catch/catch.hpp"
#include "gubg/math/InProduct.hpp"
using gubg::math::inproduct;

TEST_CASE("float", "[inprod]")
{
    float a = 2.0, b = 3.0;
    auto ip = inproduct<float>(a, b);
    REQUIRE(6.0 == ip);
}
TEST_CASE("double", "[inprod]")
{
    float a = 2.0, b = 3.0;
    auto ip = inproduct<double>(a, b);
    REQUIRE(6.0 == ip);
}
TEST_CASE("std::array", "[inprod]")
{
    typedef std::array<float, 3> Array;
    Array a = {1.0, 2.0, 3.0};
    Array b = {2.0, 2.0, 3.0};
    auto ip = inproduct<double>(a, b);
    REQUIRE(15.0 == ip);
}
TEST_CASE("std::tuple", "[inprod]")
{
    typedef std::tuple<float, float, int> Tuple;
    Tuple a(1.0, 2.0, 3);
    Tuple b(2.0, 2.0, 3);
    auto ip = inproduct<double>(a, b);
    REQUIRE(15.0 == ip);
}
