#ifndef HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Types.hpp"
#include "gubg/tmp/SFINAE.hpp"
#include <string>

namespace gubg
{
    namespace msgpack
    {
        class Object_itf
        {
            public:
                virtual void set(AttributeId, Nil_tag) = 0;
                virtual void set(AttributeId, const std::string &) = 0;
            private:
        };

        GUBG_CHECK_FOR_METHOD(CheckSetNil, set, void (U::*)(AttributeId, Nil_tag));
        GUBG_CHECK_FOR_METHOD(CheckSetString, set, void (U::*)(AttributeId, const std::string &));
        template <typename T>
            class Object: public Object_itf
        {
            public:
                Object(T &t):obj_(t){}
                virtual void set(AttributeId aid, Nil_tag nil) { typedef typename CheckSetNil<T>::Value HM; set_(HM(), aid, nil); }
                virtual void set(AttributeId aid, const std::string &str) { typedef typename CheckSetString<T>::Value HM; set_(HM(), aid, str); }

            private:
                void set_(tmp::HasNotMethod, AttributeId aid, Nil_tag nil) { }
                void set_(tmp::HasMethod, AttributeId aid, Nil_tag nil) { obj_.factory_setMember(aid, nil); }
                void set_(tmp::HasNotMethod, AttributeId aid, const std::string &str) { }
                void set_(tmp::HasMethod, AttributeId aid, const std::string &str) { obj_.factory_setMember(aid, str); }
                T &obj_;
        };

        template <typename T>
            Object<T> *wrap(T &t){return new Object<T>(t);}
    }
}

#endif
