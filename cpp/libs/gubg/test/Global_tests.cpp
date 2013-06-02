#include "gubg/Global.hpp"
using namespace gubg;

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
struct A
{
    A():
        v(42)
    {
        S();
        L("ctor");
    }
    ~A()
    {
        S();
        L("dtor");
    }
    void f()
    {
        S();
        L("f()" << v);
    }
    int v;
};

Global<A> a;

int main()
{
    a.init();
    a->f();
    return 0;
}
#include "gubg/log/end.hpp"
