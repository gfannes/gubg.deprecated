#ifndef HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Types.hpp"
#include "gubg/msgpack/Primitives.hpp"
#include "gubg/tmp/SFINAE.hpp"

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
            private:
                GUBG_CHECK_FOR_METHOD(CheckSetNil, set, void (U::*)(AttributeId, Nil_tag));
                GUBG_CHECK_FOR_METHOD(CheckSetLong, set, void (U::*)(AttributeId, long));
                GUBG_CHECK_FOR_METHOD(CheckSetString, set, void (U::*)(AttributeId, const String &));

            public:
                Object(T &t):obj_(t){}
                virtual void set(AttributeId aid, Nil_tag nil) { typedef typename CheckSetNil<T>::Value HM; set_(HM(), aid, nil); }
                virtual void set(AttributeId aid, long v) { typedef typename CheckSetLong<T>::Value HM; set_(HM(), aid, v); }
                virtual void set(AttributeId aid, const String &str) { typedef typename CheckSetString<T>::Value HM; set_(HM(), aid, str); }

            private:
                void set_(tmp::HasNotMethod, AttributeId aid, Nil_tag nil) { }
                void set_(tmp::HasMethod, AttributeId aid, Nil_tag nil) { obj_.msgpack_set(aid, nil); }
                void set_(tmp::HasNotMethod, AttributeId aid, long v) {  obj_.msgpack_set(aid, v); }
                void set_(tmp::HasMethod, AttributeId aid, long v) { obj_.msgpack_set(aid, v); }
                void set_(tmp::HasNotMethod, AttributeId aid, const String &str) { }
                void set_(tmp::HasMethod, AttributeId aid, const String &str) { obj_.msgpack_set(aid, str); }
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
