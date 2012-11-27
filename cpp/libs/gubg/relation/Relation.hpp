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
//                protected:
                    typedef std::shared_ptr<T> Ptr_;
                    typedef std::vector<Ptr_> Childs;

                    Childs childs_;

                public:
                    void addRelation(Ptr_ el){childs_.push_back(el);}
                    size_t nrRelations() const {return childs_.size();}
            };

        template <typename From, typename To>
            void relate(std::shared_ptr<From> from, std::shared_ptr<To> to)
            {
                from->Many<To>::addRelation(to);
            }

        template <typename To, typename From>
            size_t nrRelations(std::shared_ptr<From> from)
            {
                return from->Many<To>::nrRelations();
            }

        template <typename From, typename To>
            class Traits
            {
                private:
                    typedef char CanRelate_;
                    class CannotRelate_{char dummy[2];};
                    //The basic trick is to check which of the following overloads is chosen
                    //The Test(...) has lowest priority, the compiler will first try everything possible to convert MakeDerived() into const Base &
                    static CanRelate_ Test(const Many<To> &);
                    static CannotRelate_ Test(...);
                    static From &MakeFrom();
                public:
                    static const bool CanRelate = (sizeof(Test(MakeFrom())) == sizeof(CanRelate_));
            };
    }
}

#endif
