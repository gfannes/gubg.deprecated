#include "catch/catch.hpp"
#include "gubg/tuple/ForEach.hpp"
#include <tuple>
#include <string>

namespace  { 
    struct Quad
    {
        template <typename T>
            void operator()(T &t) const
            {
                t *= t;
            }
        void operator()(std::string &t) const
        {
            t.append(t);
        }
    };
} 

TEST_CASE("ForEach", "[tuple]")
{
    std::tuple<int, double, std::string> tp;
    std::get<0>(tp) = 2;
    std::get<1>(tp) = 3;
    std::get<2>(tp) = "str";
    Quad quad;
    gubg::tuple::for_each(tp, quad);
    REQUIRE(std::get<0>(tp) == 4);
    REQUIRE(std::get<1>(tp) == 9.0);
    REQUIRE(std::get<2>(tp) == "strstr");
}
