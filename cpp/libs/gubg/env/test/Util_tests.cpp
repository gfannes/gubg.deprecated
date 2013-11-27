#include "gubg/env/Util.hpp"
#include "gubg/Testing.hpp"

#include <regex>

using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    string str;
    TEST_FALSE(env::expand(str, "$NO_EXIST/sdks"));
    TEST_TRUE(env::expand(str, "$HOME/sdks"));
    L(str);
    TEST_EQ("/home/gfannes/sdks", str);
    TEST_TRUE(env::expand(str, "aaa/$HOME/bbb/$HOME/ccc"));
    L(str);
    TEST_EQ("aaa//home/gfannes/bbb//home/gfannes/ccc", str);
    return 0;
}
#include "gubg/log/end.hpp"
