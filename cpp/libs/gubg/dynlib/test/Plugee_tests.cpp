#include "catch/catch.hpp"
#include "gubg/dynlib/Plugee.hpp"
#include "A.hpp"
using namespace gubg::dynlib;
using namespace gubg::file;

TEST_CASE("Plugee", "[dynlib]")
{
    Plugee plugee(File("plugin_A.dll"));
    A_itf *a = 0;
    a = plugee.create<A_itf>();
    REQUIRE(!!a);
}
