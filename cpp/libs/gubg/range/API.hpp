#ifndef HEADER_gubg_range_API_hpp_ALREADY_INCLUDED
#define HEADER_gubg_range_API_hpp_ALREADY_INCLUDED

#include "gubg/range/Array.hpp"

namespace gubg { namespace range { 

    //The range abstract API
    template <typename T>
        class API
        {
            public:
                typedef T value_type;

                virtual ~API() = 0;

                virtual bool empty() const = 0;

                virtual T &front() const = 0;
                virtual Array<T> front_array() const = 0;

                virtual void pop_front() = 0;
                virtual void pop_front(std::size_t) = 0;

                virtual bool resize(std::size_t) = 0;
        };

    //Provides the range API, mixin style
    template <typename Obj>
        class API_mixin: public Obj, public API
    {
        public:
            //The range API implementation
            typedef Obj::value_type value_type;

            virtual ~API_mixin() {}

            virtual bool empty() const { return Obj::empty(); }

            virtual T &front() const { return Obj::front(); }
            virtual Array<T> front_array() const { return Obj::front_array(); }

            virtual void pop_front() { Obj::pop_front(); }
            virtual void pop_front(std::size_t nr) { Obj::pop_front(nr); }

            virtual bool resize(std::size_t nr) { return Obj::resize(nr); }
    };

    //Wraps Obj, providing it with the virtual range API
    template <typename Obj>
        class API_ref: public API
    {
        public:
            API_ref(Obj &obj): obj_(obj) {}

            //The range API implementation
            typedef Obj::value_type value_type;

            virtual ~API_ref() {}

            virtual bool empty() const { return obj_.empty(); }

            virtual T &front() const { return obj_.front(); }
            virtual Array<T> front_array() const { return obj_.front_array(); }

            virtual void pop_front() { obj_.pop_front(); }
            virtual void pop_front(std::size_t nr) { obj_.pop_front(nr); }

            virtual bool resize(std::size_t nr) { return obj_.resize(nr); }

        private:
            Obj &obj_;
    };

} } 

#endif
