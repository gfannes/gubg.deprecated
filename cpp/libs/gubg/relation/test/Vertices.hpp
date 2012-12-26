#ifndef HEADER_gubg_relation_test_Vertices_hpp_ALREADY_INCLUDED
#define HEADER_gubg_relation_test_Vertices_hpp_ALREADY_INCLUDED

#include "gubg/relation/Relation.hpp"

template <typename T>
struct Shareble
{
    typedef std::shared_ptr<T> Ptr;
};

struct A: Shareble<A>
{
    std::string to_s() const {return "A";}
};
struct B: Shareble<B>, gubg::relation::Many<A>
{
    std::string to_s() const {return "B";}
};
struct C: Shareble<C>, gubg::relation::Many<B>, gubg::relation::Many<A>
{
    std::string to_s() const {return "C";}
};

#endif
