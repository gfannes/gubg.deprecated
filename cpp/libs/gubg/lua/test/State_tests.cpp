#include "gubg/lua/State.hpp"
#include "gubg/Testing.hpp"
using namespace gubg::lua;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
	TEST_TAG(main);
	State s;
	s = State::create();
	TEST_OK(s.execute("print(\"Hello lua\")\nabc = 123"));
	long nr;
	TEST_OK(s.get(nr, "abc"));
	L(STREAM(nr));
	return 0;
}
#include "gubg/log/end.hpp"
