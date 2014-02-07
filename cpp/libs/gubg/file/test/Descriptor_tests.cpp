#include "gubg/file/Descriptor.hpp"
#include "gubg/Testing.hpp"
using namespace gubg::file;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
namespace 
{
    class MySelect: public Select
    {
        public:
            virtual void select_ready(Descriptor d, EventType et)
            {
                S();L(d << " is ready for some action: " << to_hr(et));
                switch (et)
                {
                    case EventType::Open:
                        {
                            Descriptor c;
                            d.accept(c);
                            add(c, AccessMode::Read);
                        }
                        break;
                    case EventType::Read:
                        {
                            std::string str(1024, '?');
                            d.read(str);
                            L(str);
                        }
                        break;
                }
            }
        private:
    };
}
int main()
{
    TEST_TAG(main);
    if (false)
    {
        TEST_TAG(file);
        auto d = Descriptor::listen(File("/dev/ttyACM0"), AccessMode::ReadWrite);
        TEST_TRUE(d.valid());
        MySelect s;
        s.add(d, AccessMode::Read);
        s.process(std::chrono::seconds(10));
        Descriptor c;
        d.accept(c);
        TEST_TRUE(c.valid());
        s.process(std::chrono::seconds(10));
    }
    if (false)
    {
        TEST_TAG(file);

        auto std_in = Descriptor::std_in();
        TEST_TRUE(std_in.valid());

        MySelect s;
        s.add(std_in, AccessMode::Read);
        s.process(std::chrono::seconds(10));
    }
    if (false)
    {
        TEST_TAG(socket);
        auto d = Descriptor::listen(1234);
        TEST_TRUE(d.valid());
        MySelect s;
        s.add(d, AccessMode::Read);
        for (int i = 0; i < 10; ++i)
        {
            S();L(i);
            s.process(std::chrono::seconds(60));
        }
    }
	if (true)
	{
		TEST_TAG(connect);
		for (int i = 0; i < 10; ++i)
		{
			auto d = Descriptor::connect("192.168.205.123", 1234);
			TEST_TRUE(d.valid());

			const std::string msg(1024, '!');
			size_t s = 0;
			TEST_OK(d.write(s, msg.substr(s)));
			while (false and s < msg.size())
			{
				size_t tmp;
				TEST_OK(d.write(tmp, msg.substr(s)));
				s += tmp;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
    return 0;
}
#include "gubg/log/end.hpp"
