#include "catch/catch.hpp"
#include "gubg/data/Block.hpp"
#include <iostream>
using namespace gubg::data;

TEST_CASE("Block", "[data]")
{
    SECTION("ctor")
    {
        Block<double, std::string> block;
        REQUIRE(block.size() == 0);
    }
    SECTION("resize")
    {
        Block<int, double, std::string> block;
        block.resize(42);
        REQUIRE(block.size() == 42);
        std::cout << block << std::endl;
    }
}
