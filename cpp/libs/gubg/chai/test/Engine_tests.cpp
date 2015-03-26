#include "gubg/chai/Engine.hpp"
#include "catch/catch.hpp"
using namespace gubg;

TEST_CASE("Engine tests", "[chai]")
{
    auto eng = chai::create_engine();
    REQUIRE(!!eng);
}
