#include "gubg/Testing.hpp"
#define GUBG_LOG
#include "gubg/relation/Vertices.hpp"
#include "gubg/l.hpp"
#include "test/Vertices.hpp"
using namespace gubg::relation;
using namespace std;

template <typename... Vs>
struct Childs: Childs_crtp<Childs<Vs...>, Vs...>
{
    template <typename V>
    void operator()(V &v)
    {
        L("\tChild::" << v.to_s() << " " << &v);
    }
};

template <typename... Vs>
struct Vertices: Vertices_crtp<Vertices<Vs...>, Vs...>
{
    Childs<Vs...> childs;
    template <typename V>
    void operator()(V &v)
    {
        L("V::" << v.to_s() << " " << &v);
        childs.each(v);
    }
};

int main()
{
    TEST_TAG(main);
    A::Ptr a(new A);
    A::Ptr aa(new A);
    B::Ptr b(new B);
    B::Ptr bb(new B);
    C::Ptr c(new C);
    b->addRelation(a);
    c->Many<A>::addRelation(a);
    c->Many<B>::addRelation(b);
    c->Many<B>::addRelation(bb);
    relate(c, b);

    Vertices<A, B, C> vs;
    vs.Many<A>::addRelation(a);
    vs.Many<A>::addRelation(aa);
    vs.Many<B>::addRelation(b);
    vs.Many<C>::addRelation(c);
    vs.iterate();
    return 0;
}