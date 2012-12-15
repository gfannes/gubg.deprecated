#include "gubg/Testing.hpp"
#include "gubg/tmp/FindBaseByName.hpp"
#include "gubg/l.hpp"
#include <string>
using namespace gubg::tmp;

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
    C(){L("C ctor");}
};
struct E: C
{
    typedef TypeList<NameType<EC_tag, C> > Pubs;
    E(){L("E ctor");}
};
struct T: C
{
    typedef TypeList<NameType<TC_tag, C> > Pubs;
    T(){L("T ctor");}
};
struct LL: E, T
{
    typedef TypeList<NameType<E_tag, E>, TypeList<NameType<T_tag, T> > > Pubs;
    LL(){L("LL ctor");}
};

int main()
{
    TEST_TAG(Inheritance);
    L("We lookup recursively the type named \"E_tag\"");
    FindBaseByName<E_tag, LL::Pubs>::Type e;

    L("We lookup recursively the type named \"EC_tag\"");
    FindBaseByName<EC_tag, LL::Pubs>::Type et;

    L("We lookup recursively the type named \"WTF_tag\"");
    FindBaseByName<WTF_tag, LL::Pubs>::Type wtf;
    return 0;
}