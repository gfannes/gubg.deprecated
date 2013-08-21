#include "gubg/file/Descriptor.hpp"
#include "gubg/Testing.hpp"
using namespace gubg::file;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
namespace 
{
    class S: public Select
    {
        public:
                virtual void select_readyToRead(Descriptor d)
                {
                    S();L(d << " is ready to read");
                }
                virtual void select_readyToWrite(Descriptor d)
                {
                    S();L(d << " is ready to write");
                }
        private:
    };
}
int main()
{
    TEST_TAG(main);
    if (true)
    {
        TEST_TAG(file);
        auto d = Descriptor::listen(File("/dev/ttyACM0"), AccessMode::ReadWrite);
        TEST_TRUE(d.valid());
        S s;
        s.add(d, AccessMode::Read);
        s(std::chrono::seconds(1));
        Descriptor c;
        d.accept(c);
        TEST_TRUE(c.valid());
    }
    if (false)
    {
        TEST_TAG(socket);
        auto d = Descriptor::listen(1234);
        TEST_TRUE(d.valid());
        S s;
        s.add(d, AccessMode::Read);
        for (int i = 0; i < 10; ++i)
        {
            s(std::chrono::seconds(60));
            Descriptor c;
            d.accept(c);
        }
    }
    return 0;
}
#include "gubg/log/end.hpp"
