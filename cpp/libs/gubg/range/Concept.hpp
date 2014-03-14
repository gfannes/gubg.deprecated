#ifndef HEADER_gubg_range_Concept_hpp_ALREADY_INCLUDED
#define HEADER_gubg_range_Concept_hpp_ALREADY_INCLUDED

#include "gubg/range/Array.hpp"
#include <cstddef>

namespace gubg { namespace range { 

    char f();
    const char &g();

    template <typename Value>
        class Concept
        {
            public:
                typedef Value value_type;

                bool empty() const;

                //We assume that the range returns a reference, so we can
                //take its address if necessary
                const value_type &front() const;
                Array<Value> front_array() const;

                void pop_front();
                void pop_front(size_t);

            private:
        };

    //Default implementation
    template <typename Value>
        Array<Value> Concept<Value>::front_array() const
        {
            Array<Value> res;
            return res;
        }
    template <typename Value>
        void Concept<Value>::pop_front(size_t size)
        {
            for (; size > 0; --size)
                pop_front();
        }
} } 

#endif
