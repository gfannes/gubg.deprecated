#include "gubg/OrderedSet.hpp"
#include "gubg/Testing.hpp"
using namespace gubg;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(OrderedSet);
    OrderedSet<int> os;
    os.insert(0);
    os.insert(2);
    os.insert(1);
    for (auto &e: os)
        L(e);
    return 0;
}
#include "gubg/log/end.hpp"
