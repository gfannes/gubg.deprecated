#include "gubg/Output.hpp"
#include "gubg/Testing.hpp"
#include <ostream>

namespace 
{
	struct Decorator
	{
		void before(std::ostream &os)
		{
			os << "$ ";
		}
		void after(std::ostream &os)
		{
			os << std::endl;
		}
	};
	class A: public gubg::Output<Decorator>
	{
		public:
			A(): Output("a.log") {}
		private:
	};
}

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
	TEST_TAG(main);
	{
		TEST_TAG(raw);
		gubg::output<Decorator>() << "test" << 123 << "test" << __cplusplus;
		gubg::output<Decorator>() << "test" << 123 << "test";
		gubg::output<Decorator>("test", 1) << "test" << 123 << "test";
		gubg::StreamMgr::instance().get(gubg::file::File("test")).setLevel(2);
		gubg::output<Decorator>("test", 1) << "test" << 123 << "test";
	}
	{
		TEST_TAG(klass);
		A a;
		a.output() << "test";
	}
	return 0;
}
#include "gubg/log/end.hpp"
