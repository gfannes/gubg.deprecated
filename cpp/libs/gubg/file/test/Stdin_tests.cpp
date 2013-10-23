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

    auto stdin = Descriptor::stdin();
    TEST_TRUE(stdin.valid());

    MySelect s;
    s.add(stdin, AccessMode::Read);
    s.process(std::chrono::seconds(10));
    return 0;
}
#include "gubg/log/end.hpp"
