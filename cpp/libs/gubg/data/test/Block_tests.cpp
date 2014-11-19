#include "catch/catch.hpp"
#include "gubg/data/Block.hpp"
#include <iostream>
using namespace gubg::data;
using gubg::file::File;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
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
    SECTION("transform")
    {
        typedef Block<int, double, std::string> B;
        B b(4);
        b.transform<2>([](const B::Record &r){return "string";});
        std::cout << b << std::endl;
    }
    SECTION("expand with copy")
    {
        S();
        typedef Block<int, double, std::string> B;
        B b(4);
        b.transform<2>([](const B::Record &r){return "string";});
        auto nb = b.expand<int>([](const B::Record &r){return 23;});
        REQUIRE(b.size() == 4);
        REQUIRE(nb.size() == 4);
        std::cout << b << std::endl;
        std::cout << nb << std::endl;
    }
    SECTION("expand with swap")
    {
        S();
        typedef Block<int, double, std::string> B;
        B b(4);
        b.transform<2>([](const B::Record &r){return "string";});
        auto nb = b.swap_expand<int>([](const B::Record &r){return 23;});
        REQUIRE(b.size() == 0);
        REQUIRE(nb.size() == 4);
        std::cout << b << std::endl;
        std::cout << nb << std::endl;
    }
    SECTION("load data from file")
    {
        typedef Block<int, double, std::string> B;
        B b;
        REQUIRE(b.load(File("data.txt"), ' ') == ReturnCode::OK);
        REQUIRE(b.size() == 2);
        std::cout << b << std::endl;
    }
}
#include "gubg/log/end.hpp"
