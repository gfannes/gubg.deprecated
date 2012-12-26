#define GUBG_LOG
#include "gubg/env/Variables.hpp"
#include "gubg/Testing.hpp"
#include "gubg/l.hpp"
using namespace gubg::env;
using namespace std;

int main()
{
    TEST_TAG(main);
    Variables vars;
    vars = Variables::shell();
    string v;
    TEST_TRUE(vars.get(v, "SHELL"));
    L(v);
    TEST_TRUE(vars.get(v, "HOME"));
    L(v);
    return 0;
}
