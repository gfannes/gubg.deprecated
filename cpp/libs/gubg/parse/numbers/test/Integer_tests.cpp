#include "gubg/parse/numbers/Integer.hpp"
#include "gubg/Testing.hpp"
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    {
        TEST_TAG(pos);
        L(STREAM(sizeof(long), sizeof(long long)));
        auto a = "1234567890bla";
        long aa = 1234567890, bb = 0;
        size_t l = 13;
        TEST_TRUE(gubg::parse::numbers::read(bb, a, l));
        TEST_EQ(aa, bb);
        TEST_EQ(10, l);
    }
    {
        TEST_TAG(stress);
        const size_t Nr = 10000000;
        vector<long>   ls(Nr);
        iota(ls.begin(), ls.end(), 0);
        vector<string> strs(Nr);
        {
            SSS("Generating numbers");
            ostringstream oss;
            transform(ls.begin(), ls.end(), strs.begin(), [&](long l){oss.str(""); oss << l; return oss.str();});
        }

        long bb;
        size_t s;
        long sum_mine = 0, sum_strtol = 0;
        {
            SSS("mine");
            for (size_t i = 0; i < Nr; ++i)
            {
                gubg::parse::numbers::read(bb, strs[i].c_str(), s = strs[i].size());
                sum_mine += bb;
            }
        }
        {
            SSS("strtol");
            char *e;
            for (size_t i = 0; i < Nr; ++i)
            {
                bb = std::strtol(strs[i].c_str(), &e, 10);
                sum_strtol += bb;
            }
        }
        TEST_EQ(sum_mine, sum_strtol);
    }
    return 0;
}
#include "gubg/log/end.hpp"
