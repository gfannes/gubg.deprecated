#include "gubg/Context.hpp"
#include "gubg/l.hpp"
using namespace gubg;
using namespace std;

namespace 
{
    struct C
    {
        C(){L("ctor");}
        ~C(){L("dtor");}
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
