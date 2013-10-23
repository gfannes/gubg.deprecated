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
                    S();L(d << " is ready " << STREAM((int)et));
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
    if (true)
    {
        TEST_TAG(file);
        auto d = Descriptor::listen_stdin();
        TEST_TRUE(d.valid());
        MySelect s;
        s.add(d, AccessMode::Read);
        s.process(std::chrono::milliseconds(500));
        Descriptor c;
        d.accept(c);
        TEST_TRUE(c.valid());
        MySelect s2;
        s2.add(c, AccessMode::Read);
        s2.process(std::chrono::seconds(10));
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
            s.process(std::chrono::seconds(60));
            Descriptor c;
            d.accept(c);
        }
    }
    return 0;
}
#include "gubg/log/end.hpp"
