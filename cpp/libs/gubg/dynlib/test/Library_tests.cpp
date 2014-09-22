#include "catch/catch.hpp"
#include "gubg/dynlib/Library.hpp"
using namespace gubg::dynlib;
using namespace gubg::file;

TEST_CASE("open", "[dynlib]")
{
	Library lib;
	SECTION("default ctor should be invalid") { REQUIRE_FALSE(lib.valid()); }
	SECTION("fail to load an unexisting dll")
	{
		lib = Library(File("unexisting.dll"));
		REQUIRE_FALSE(lib.valid());
	}
	SECTION("load an existing dll")
	{
		lib = Library(File("test.dll"));
		REQUIRE(lib.valid());
	}
}
