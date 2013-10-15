#include "gubg/Context.hpp"
using namespace gubg;
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
namespace 
{
    struct C
    {
        C(){S();L("ctor");}
        ~C(){S();L("dtor");}
    };
}

int main()
{
    auto c = Context<C>::create();
    Context<C> cc(c);
    cc = c;
    cc = c.context;
    cc.set(c);
    cc.set(c.context);
    cc.clear();
    c.clear();
    Context<C> ccc(make_shared<C>());
    cc = ccc;
    return 0;
}
#include "gubg/log/end.hpp"
