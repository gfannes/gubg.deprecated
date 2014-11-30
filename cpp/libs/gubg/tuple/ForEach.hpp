#ifndef HEADER_gubg_tuple_ForEach_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tuple_ForEach_hpp_ALREADY_INCLUDED

#include "gubg/tmp/IntType.hpp"
#include <tuple>

namespace gubg { namespace tuple { 

    namespace details { 
        template <size_t Nr>
            struct Process
            {
                template <typename Tuple, typename Ftor>
                    static void process(Tuple &tp, Ftor &ftor)
                    {
                        Process<Nr-1>::process(tp, ftor);
                        ftor(std::get<Nr-1>(tp));
                    }
                template <typename Tuple, typename Ftor>
                    static void process(Tuple &a, Tuple &b, Ftor &ftor)
                    {
                        Process<Nr-1>::process(a, b, ftor);
                        ftor(std::get<Nr-1>(a), std::get<Nr-1>(b));
                    }
                template <typename Tuple, typename Ftor>
                    static void process_with_ix(Tuple &tp, Ftor &ftor)
                    {
                        Process<Nr-1>::process_with_ix(tp, ftor);
                        tmp::IntType<Nr-1> ix;
                        ftor(std::get<Nr-1>(tp), ix);
                    }
            };
        template <>
            struct Process<0>
            {
                template <typename Tuple, typename Ftor>
                    static void process(Tuple &, Ftor &) { }
                template <typename Tuple, typename Ftor>
                    static void process(Tuple &, Tuple &, Ftor &) { }
                template <typename Tuple, typename Ftor>
                    static void process_with_ix(Tuple &, Ftor &) { }
            };
    } 

    template <typename Tuple, typename Ftor>
        void for_each(Tuple &tp, Ftor &ftor)
        {
            constexpr size_t tuple_size =  std::tuple_size<Tuple>::value;
            details::Process<tuple_size>::process(tp, ftor);
        }
    template <typename Tuple, typename Ftor>
        void for_each(Tuple &a, Tuple &b, Ftor &ftor)
        {
            constexpr size_t tuple_size =  std::tuple_size<Tuple>::value;
            details::Process<tuple_size>::process(a, b, ftor);
        }
    template <typename Tuple, typename Ftor>
        void for_each_with_ix(Tuple &tp, Ftor &ftor)
        {
            constexpr size_t tuple_size =  std::tuple_size<Tuple>::value;
            details::Process<tuple_size>::process_with_ix(tp, ftor);
        }

} } 

#endif
