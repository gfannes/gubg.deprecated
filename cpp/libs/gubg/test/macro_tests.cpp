#include "catch/catch.hpp"
#include "gubg/Macro.hpp"
#include <iostream>
using namespace std;

TEST_CASE("GET_ARG_N", "[macro]")
{
    {
        auto i = GUBG_GET_ARG_1((1));
        REQUIRE(i == 1);
    }
    {
        auto i = GUBG_GET_ARG_2((1, 2));
        REQUIRE(i == 2);
    }
    {
        auto i = GUBG_GET_ARG_11((1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11));
        REQUIRE(i == 11);
    }
}

TEST_CASE("STREAM", "[macro]")
{
    int a = 1, b = 2;
    cout << STREAM(a, b) << endl;
}
