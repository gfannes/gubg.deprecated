#include "catch/catch.hpp"
#include "gubg/tmp/IntType.hpp"

TEST_CASE("IntType", "[tmp]")
{
    typedef gubg::tmp::IntType<42> T;
    REQUIRE(T::value == 42);
}
