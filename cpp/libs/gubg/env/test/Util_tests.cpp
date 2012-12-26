#define GUBG_LOG
#include "gubg/env/Util.hpp"
#include "gubg/Testing.hpp"
#include "gubg/l.hpp"
using namespace gubg;
using namespace std;

int main()
{
    TEST_TAG(main);
    auto str = env::expand("$HOME/sdks");
    L(str);
    TEST_EQ("/home/gfannes/sdks", str);
    str = env::expand("aaa/$HOME/bbb/$HOME/ccc");
    L(str);
    TEST_EQ("aaa//home/gfannes/bbb//home/gfannes/ccc", str);
    return 0;
}
