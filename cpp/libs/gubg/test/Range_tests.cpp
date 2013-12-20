#include "gubg/Range.hpp"
#include "gubg/Testing.hpp"
#include <vector>
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    vector<int> v;
    auto r = gubg::make_range(v);
    TEST_TRUE(r.empty());
    v.push_back(0);
    r = gubg::make_range(v);
    TEST_FALSE(r.empty());
    gubg::Range<vector<int>::iterator> r2;
    r2 = r;
    TEST_FALSE(r2.empty());
    TEST_TRUE(r.contains(v.begin()));
    TEST_FALSE(r.contains(v.end()));
    return 0;
}
#include "gubg/log/end.hpp"
