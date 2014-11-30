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
    SECTION("push_back record")
    {
        typedef Block<int, double, std::string> B;
        B b;
        REQUIRE(b.size() == 0);
        b.push_back(B::Record(42, 0.42, "answer"));
        REQUIRE(b.size() == 1);
        std::cout << b << std::endl;
    }
    SECTION("iterator default ctor")
    {
        typedef Block<int, double, std::string> B;
        B b;
        B::const_iterator it;
    }
    SECTION("iterator begin/end on empty block")
    {
        typedef Block<int, double, std::string> B;
        B b;
        auto begin = b.begin();
        auto end = b.end();
        REQUIRE(begin == end);
        REQUIRE(!(begin != end));
    }
    SECTION("iterator begin/end on non-empty block")
    {
        typedef Block<int, double, std::string> B;
        B b;
        REQUIRE(b.load(File("data.txt"), ' ') == ReturnCode::OK);
        auto begin = b.begin();
        auto end = b.end();
        REQUIRE(!(begin == end));
        REQUIRE(begin != end);
    }
    SECTION("iterator dereference")
    {
        typedef Block<int, double, std::string> B;
        B b;
        REQUIRE(b.load(File("data.txt"), ' ') == ReturnCode::OK);
        auto r = *b.begin();
        REQUIRE(1 == std::get<0>(r));
        REQUIRE(2.4 == std::get<1>(r));
        REQUIRE(std::string("abc") == std::get<2>(r));
    }
    SECTION("iterator increment and dereference")
    {
        typedef Block<int, double, std::string> B;
        B b;
        REQUIRE(b.load(File("data.txt"), ' ') == ReturnCode::OK);
        auto it = b.begin();
        {
            ++it;
            auto r = *it;
            REQUIRE(2 == std::get<0>(r));
            REQUIRE(4.2 == std::get<1>(r));
            REQUIRE(std::string("aoeuaoeuaoeuaoeuaoeuaoeu") == std::get<2>(r));
        }
        {
            --it;
            auto r = *it;
            REQUIRE(1 == std::get<0>(r));
            REQUIRE(2.4 == std::get<1>(r));
            REQUIRE(std::string("abc") == std::get<2>(r));
        }
    }
}
#include "gubg/log/end.hpp"
