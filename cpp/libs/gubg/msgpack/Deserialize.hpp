#ifndef HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Types.hpp"
#include "gubg/msgpack/Primitives.hpp"

#define GUBG_MODULE_ "Deserialize"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace msgpack
    {
        template <typename String>
            class Object_itf
            {
                public:
                    Object_itf(): ptr_(0) {}
                    Object_itf(void *ptr): ptr_(ptr) {}

                    virtual void set(AttributeId, Nil_tag) {S();L("nil");}
                    virtual void set(AttributeId, long v) {S();L(v);}
                    virtual void set(AttributeId, const String &str) {S();L(str);}
                    AttributeId aid;
                    TypeId tid;

                protected:
                    //We will store the address of the wrapped object here
                    void *ptr_;
            };

        template <typename String, typename T>
            class Object: public gubg::msgpack::Object_itf<String>
        {
            private:
                typedef gubg::msgpack::Object_itf<String> Base;

            public:
                Object(T &t): Base(&t) {}
                virtual void set(AttributeId aid, Nil_tag nil)       { obj_().msgpack_set(aid, nil); }
                virtual void set(AttributeId aid, long v)            { obj_().msgpack_set(aid, v); }
                virtual void set(AttributeId aid, const String &str) { obj_().msgpack_set(aid, str); }

            private:
                T &obj_() {return *reinterpret_cast<T*>(Base::ptr_);}
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
#include "gubg/log/end.hpp"

#endif
