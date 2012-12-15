#include "gubg/Testing.hpp"
#define GUBG_LOG
#include "gubg/relation/Relation.hpp"
#include "gubg/l.hpp"
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

struct DFS: DFS_crtp<DFS, A>
{
    void dfs_discover(A &a)
    {
        L(&a);
    }
};
struct DFS2: DFS_crtp<DFS2, A, B>
{
    void dfs_discover(A &a)
    {
        L("A::" << &a);
    }
    void dfs_discover(B &b)
    {
        L("B::" << &b);
    }
};

int main()
{
    TEST_TAG(main);
    A::Ptr a(new A);
    A::Ptr aa(new A);
    B::Ptr b(new B);
    C::Ptr c(new C);
    b->addRelation(a);
    c->Many<A>::addRelation(a);
    c->Many<B>::addRelation(b);
    c->Many<B>::addRelation(b);
    relate(c, b);
    TEST_EQ(1, b->Many<A>::nrRelations());
    TEST_EQ(1, c->Many<A>::nrRelations());
    TEST_EQ(3, c->Many<B>::nrRelations());
    TEST_EQ(3, nrRelations<B>(c));
    TEST_TRUE((Traits<B, A>::CanRelate));
    TEST_FALSE((Traits<A, B>::CanRelate));
    TEST_TRUE((Traits<C, A>::CanRelate));
    TEST_TRUE((Traits<C, B>::CanRelate));

    DFS dfs;
    dfs.addRelation(a);
    dfs.addRelation(a);
    dfs.iterate();

    DFS2 dfs2;
    dfs2.Many<A>::addRelation(a);
    dfs2.Many<A>::addRelation(aa);
    dfs2.Many<B>::addRelation(b);
    dfs2.iterate();
    return 0;
}