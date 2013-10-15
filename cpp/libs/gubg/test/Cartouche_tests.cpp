#include "gubg/Cartouche.hpp"
#include "gubg/Testing.hpp"
using namespace gubg;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    Cartouche ct;
    ct.parse("aaa");
    return 0;
}
#include "gubg/log/end.hpp"
