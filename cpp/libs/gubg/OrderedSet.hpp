#ifndef HEADER_gubg_OrderedSet_hpp_ALREADY_INCLUDED
#define HEADER_gubg_OrderedSet_hpp_ALREADY_INCLUDED

#include <set>
#include <list>

namespace gubg
{
    template <typename T>
        class OrderedSet
        {
            private:
                typedef std::set<T> Set_;
                typedef std::list<typename Set_::const_iterator> List_;

            public:
                void clear()
                {
                    set_.clear();
                    list_.clear();
                }

                void insert(const T &e)
                {
                    auto p = set_.insert(e);
                    if (!p.second)
                        return;
                    list_.push_back(p.first);
                }
                void insert(const OrderedSet &os)
                {
                    for (auto e: os.list_)
                        insert(*e);
                }

                class const_iterator
                {
                    public:
                        const_iterator(typename List_::const_iterator it):it_(it){}
                        const_iterator(){}
                        bool operator!=(const const_iterator &rhs) const {return it_ != rhs.it_;}
                        const_iterator &operator++(){++it_; return *this;}
                        //Dereference is const since you cannot change the value for a set, the value itself is important
                        //for its place in the set
                        const T &operator*() const { return **it_; }
                    private:
                        typename List_::const_iterator it_;
                };
                const_iterator begin() const {return const_iterator(list_.begin());}
                const_iterator end() const {return const_iterator(list_.end());}

            private:
                Set_ set_;
                List_ list_;
        };
}

#endif
