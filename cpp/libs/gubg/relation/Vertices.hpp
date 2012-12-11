#ifndef HEADER_gubg_relation_Vertices_hpp_ALREADY_INCLUDED
#define HEADER_gubg_relation_Vertices_hpp_ALREADY_INCLUDED

#include "gubg/relation/Relation.hpp"
#include "gubg/tmp/If.hpp"

namespace gubg
{
    namespace relation
    {
        template <typename...Ts>
            struct Vertices_crtp;
        template <typename Receiver, typename T, typename... Ts>
            struct Vertices_crtp<Receiver, T, Ts...>: Many<T>, Vertices_crtp<Receiver, Ts...>
            {
                typedef Many<T> ManyBase;
                typedef Vertices_crtp<Receiver, Ts...> VerticesBase;
                void iterate()
                {
                    for (auto ptr: ManyBase::childs_)
                    {
                        receiver_().operator()(*ptr);
                    }
                    VerticesBase::iterate();
                }
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}
            };
        template <typename Receiver>
            struct Vertices_crtp<Receiver>
            {
                void iterate(){}
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}
            };

        struct Iterate_tag{};
        struct Skip_tag{};

        template <typename...Ts>
            struct Childs_crtp;
        template <typename Receiver, typename T, typename... Ts>
            struct Childs_crtp<Receiver, T, Ts...>: Childs_crtp<Receiver, Ts...>
            {
                typedef Childs_crtp<Receiver, Ts...> ChildsBase;
                template <typename V>
                void each(V &v)
                {
                    each_(v, typename tmp::If<Traits<V, T>::CanRelate, Iterate_tag, Skip_tag>::Type());
                    ChildsBase::each(v);
                }
                template <typename V>
                void each_(V &v, Iterate_tag)
                {
                    for (auto c: v.Many<T>::childs_)
                        receiver_().operator()(*c);
                }
                template <typename V>
                void each_(V &v, Skip_tag) { }
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}
            };
        template <typename Receiver>
            struct Childs_crtp<Receiver>
            {
                template <typename V>
                void each(V &v){}
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}
            };
    }
}

#endif
