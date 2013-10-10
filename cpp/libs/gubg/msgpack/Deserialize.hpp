#ifndef HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Types.hpp"
#include "gubg/msgpack/Primitives.hpp"

namespace gubg
{
    namespace msgpack
    {
        template <typename String>
            class Object_itf
            {
                public:
                    virtual void set(AttributeId, Nil_tag) = 0;
                    virtual void set(AttributeId, long) = 0;
                    virtual void set(AttributeId, const String &) = 0;
                    AttributeId aid;
                    TypeId tid;
                private:
            };

        template <typename String, typename T>
            class Object: public Object_itf<String>
        {
            public:
                Object(T &t):obj_(t){}
                virtual void set(AttributeId aid, Nil_tag nil) { obj_.msgpack_set(aid, nil); }
                virtual void set(AttributeId aid, long v) { obj_.msgpack_set(aid, v); }
                virtual void set(AttributeId aid, const String &str) { obj_.msgpack_set(aid, str); }

            private:
                T &obj_;
        };

        template <typename String, typename T>
            Object_itf<String> *wrap(T &t)
            {
                //We make sure to clear t before deserializing it
                t = T();
                return new Object<String, T>(t);
            }
        template <typename String, typename T>
            Object_itf<String> *wrapWithoutClear(T &t)
            {
                return new Object<String, T>(t);
            }
    }
}

#endif
