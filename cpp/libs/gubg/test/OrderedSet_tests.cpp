#include "gubg/OrderedSet.hpp"
#include "gubg/Testing.hpp"
#include "gubg/l.hpp"
using namespace gubg;

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
