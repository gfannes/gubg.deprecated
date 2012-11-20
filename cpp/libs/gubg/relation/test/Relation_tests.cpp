#include "gubg/testing/Testing.hpp"
#define GUBG_LOG
#include "gubg/relation/Relation.hpp"
using namespace gubg::relation;
using namespace std;

template <typename T>
struct Shareble
{
    typedef shared_ptr<T> Ptr;
};

struct A: Shareble<A>
{
};
struct B: Shareble<B>, Many<A>
{
};
struct C: Shareble<C>, Many<B>, Many<A>
{
};

int main()
{
    TEST_TAG(main);
    A::Ptr a(new A);
    B::Ptr b(new B);
    C::Ptr c(new C);
    b->addRelation(a);
    c->Many<A>::addRelation(a);
    c->Many<B>::addRelation(b);
    c->Many<B>::addRelation(b);
    TEST_EQ(1, b->Many<A>::nrRelations());
    TEST_EQ(1, c->Many<A>::nrRelations());
    TEST_EQ(2, c->Many<B>::nrRelations());
    return 0;
}
