#include "catch/catch.hpp"
#include "gubg/tmp/Equal.hpp"
#include <string>
using namespace gubg::tmp;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
struct T1 {};
struct T2 {};
typedef T1 T3;
struct T4: T1 {};

TEST_CASE("Equal", "[tmp]")
{
    REQUIRE((Equal<T1, T1>::Value));
    REQUIRE((Equal<T2, T2>::Value));
    REQUIRE((Equal<T3, T3>::Value));
    REQUIRE((Equal<T4, T4>::Value));
    REQUIRE((Equal<T1, T3>::Value));
    REQUIRE(!(Equal<T1, T2>::Value));
    REQUIRE(!(Equal<T2, T1>::Value));
    REQUIRE(!(Equal<T2, T3>::Value));
    REQUIRE(!(Equal<T3, T2>::Value));
    REQUIRE(!(Equal<T1, T4>::Value));
    REQUIRE(!(Equal<T4, T1>::Value));
}
#include "gubg/log/end.hpp"
