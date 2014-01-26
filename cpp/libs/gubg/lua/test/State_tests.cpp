#include "gubg/lua/State.hpp"
#include "gubg/Testing.hpp"
#include "lua.hpp"
using namespace gubg::lua;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
extern "C"
{
	int myf(lua_State *s)
	{
		SS();LL(lua_gettop(s));
		lua_pushnumber(s, 123);
		return 1;
	}
}

int main()
{
	TEST_TAG(main);
	State s;
	s = State::create();
	TEST_OK(s.execute("print(\"Hello lua\")\nabc = 123"));
	long nr;
	TEST_OK(s.get(nr, "abc"));
	L(STREAM(nr));
	TEST_KO(s.execute("myf()"));
	s.registerFunction(myf, "myf");
	TEST_OK(s.execute("local res = myf(1,2,3); print(res);"));
	return 0;
}
#include "gubg/log/end.hpp"
