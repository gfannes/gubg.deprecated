#include "gubg/trace/Scope.hpp"
using namespace gubg::trace;

int main()
{
    Scope s1(Msg(0) << "main" << 1);
    Scope s2(Msg(0) << "main" << 2);
    return 0;
}
