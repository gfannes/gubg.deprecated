#include "gubg/Buffer.hpp"
#include "gubg/Testing.hpp"
using namespace gubg;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    InputBuffer<1024> ib;
    for (int i = 0; i < 1024; ++i)
        TEST_TRUE(ib.push_back('a'));
    TEST_FALSE(ib.push_back('a'));
    return 0;
}
#include "gubg/log/end.hpp"
