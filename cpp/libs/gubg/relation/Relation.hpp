#ifndef HEADER_gubg_relation_Relation_hpp_ALREADY_INCLUDED
#define HEADER_gubg_relation_Relation_hpp_ALREADY_INCLUDED

#include <vector>
#include <memory>

namespace gubg
{
    namespace relation
    {
        template <typename T>
            struct Many
            {
                private:
                    typedef std::shared_ptr<T> Ptr_;
                    typedef std::vector<Ptr_> Ary;

                    Ary ary;

                public:
                    void addRelation(Ptr_ el){ary.push_back(el);}
                    size_t nrRelations() const {return ary.size();}
            };

        template <typename T, typename... Rest>
            struct M
            {
            };
    }
}

#endif
