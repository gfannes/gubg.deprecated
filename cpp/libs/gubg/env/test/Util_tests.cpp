#define GUBG_LOG
#include "gubg/env/Util.hpp"
#include "gubg/Testing.hpp"
#include "gubg/l.hpp"
using namespace gubg;
using namespace std;

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
