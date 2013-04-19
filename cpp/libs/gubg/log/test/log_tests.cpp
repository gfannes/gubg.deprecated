#define GUBG_MODULE "fff"
#include "gubg/log/begin.hpp"
void ff(int a, int b)
{
    SS(a, b);
    L("oaeuaoeu");
}
void f()
{
    S();
    L("oaeuaoeu");
    ff(123, 456);
}
#include "gubg/log/end.hpp"

#define GUBG_MODULE "ggg"
#include "gubg/log/begin.hpp"
void g()
{
    S();
    L("before");
    f();
    L("after");
}
void h()
{
    LL("raw logging");
    g();
}
#include "gubg/log/end.hpp"

#define GUBG_MODULE "MAIN"
#include "gubg/log/begin.hpp"
int main()
{
    LL("Before the main scope");
    S();
    f();
    g();
    h();
    return 0;
}
