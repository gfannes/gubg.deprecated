#include "catch/catch.hpp"
#include "gubg/math/stat/Median.hpp"
#include "gubg/Macro.hpp"
#include <vector>
using namespace gubg;

TEST_CASE("Median", "[stat]")
{
    typedef std::vector<float> V;

    SECTION("one element")
    {
        V v = {1.0};
        REQUIRE(1.0 == math::median(RANGE(v)));
    }
    SECTION("two elements")
    {
        V v = {1.0, 2.0};
        REQUIRE(1.5 == math::median(RANGE(v)));
    }
    SECTION("two elements different order")
    {
        V v = {2.0, 1.0};
        REQUIRE(1.5 == math::median(RANGE(v)));
    }
    SECTION("three elements")
    {
        V v = {1.0, 2.0, 3.0};
        REQUIRE(2.0 == math::median(RANGE(v)));
    }
    SECTION("three elements different order")
    {
        V v = {2.0, 1.0, 3.0};
        REQUIRE(2.0 == math::median(RANGE(v)));
    }
}
