#include "gubg/env/Variables.hpp"
#include "gubg/Testing.hpp"
using namespace gubg::env;
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
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
#include "gubg/log/end.hpp"
