#include "gubg/range/String.hpp"
#include "gubg/Testing.hpp"
using namespace std;
using namespace gubg::range;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    string str;
    Range_string r(str);
    TEST_TRUE(r.empty());
    TEST_TRUE(r.resizeBack(2));
    auto sp = r;
    TEST_FALSE(r.empty());
    r.front() = 's';
    r.popFront();
    TEST_FALSE(r.empty());
    r.front() = 'n';
    r.popFront();
    TEST_TRUE(r.empty());
    TEST_EQ('s', sp.front());
    sp.popFront();
    TEST_EQ('n', sp.front());
    sp.popFront();
    TEST_TRUE(r.empty());
    return 0;
}
#include "gubg/log/end.hpp"
