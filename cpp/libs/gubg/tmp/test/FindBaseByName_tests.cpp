#include "gubg/Testing.hpp"
#include "gubg/tmp/FindBaseByName.hpp"
#include <string>
using namespace gubg::tmp;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
//Names that will be used to indicate the different published base classes
struct E_tag{};
struct T_tag{};
struct EC_tag{};
struct TC_tag{};
struct WTF_tag{};

//Hierarchy
struct C
{
    typedef Null Pubs;
    C(){S();L("C ctor");}
};
struct E: C
{
    typedef TypeList<NameType<EC_tag, C> > Pubs;
    E(){S();L("E ctor");}
};
struct T: C
{
    typedef TypeList<NameType<TC_tag, C> > Pubs;
    T(){S();L("T ctor");}
};
struct LL_: E, T
{
    typedef TypeList<NameType<E_tag, E>, TypeList<NameType<T_tag, T> > > Pubs;
    LL_(){S();L("LL_ ctor");}
};

int main()
{
    TEST_TAG(Inheritance);
    L("We lookup recursively the type named \"E_tag\"");
    FindBaseByName<E_tag, LL_::Pubs>::Type e;

    L("We lookup recursively the type named \"EC_tag\"");
    FindBaseByName<EC_tag, LL_::Pubs>::Type et;

    L("We lookup recursively the type named \"WTF_tag\"");
    FindBaseByName<WTF_tag, LL_::Pubs>::Type wtf;
    return 0;
}
#include "gubg/log/end.hpp"
