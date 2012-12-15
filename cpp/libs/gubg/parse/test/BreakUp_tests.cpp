#include "gubg/Testing.hpp"
#include "gubg/parse/BreakUp.hpp"
#include "gubg/l.hpp"
#include <string>
using namespace std;

struct Breaker
{
    template <typename It>
    bool operator()(It b, It m, It e)
    {
        if (m == e)
        {
            L("Last: " << string(b, m));
            return true;
        }
        if (*m == '|')
        {
            L(string(b, m));
            return true;
        }
        return false;
    }
};

int main()
{
    TEST_TAG(BreakUp);
    string str("a|b|c");
    Breaker breaker;
    gubg::parse::breakUp(str.begin(), str.end(), breaker);
    return 0;
}