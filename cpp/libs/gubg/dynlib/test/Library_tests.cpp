#include "catch/catch.hpp"
#include "gubg/dynlib/Library.hpp"
#include <iostream>
using namespace gubg::dynlib;
using namespace gubg::file;
using namespace std;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
TEST_CASE("open", "[dynlib]")
{
    S();
	Library lib;
	SECTION("default ctor should be invalid") { REQUIRE_FALSE(lib.valid()); }
	SECTION("fail to load an unexisting dll")
	{
		lib = Library(File("unexisting.dll"));
		REQUIRE_FALSE(lib.valid());
	}

    lib = Library(File("test.dll"));
	SECTION("load an existing dll") { REQUIRE(lib.valid()); }
    SECTION("get a symbol")
    {
        auto sym = lib.get("answer");
        REQUIRE(!!sym);
        cout << sym << endl;
        L(sym);
        typedef int(*F_ptr)(void);
        F_ptr f_ptr = (F_ptr)sym;
        cout << f_ptr() << endl;
        REQUIRE(42 == f_ptr());
    }
}
#include "gubg/log/end.hpp"
