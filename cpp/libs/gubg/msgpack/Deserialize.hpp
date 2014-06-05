#ifndef HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Types.hpp"
#include "gubg/msgpack/Primitives.hpp"
#include "gubg/array.hpp"
#include <cassert>

#define GUBG_MODULE_ "Deserialize"
#include "gubg/log/begin.hpp"
namespace gubg { namespace msgpack {

    //Wraps a reference to an object and a few methods (msgpack_set()), type-erasure style.
    //The non-virtual methods will call into those of the object passed during construction.
    template <typename String>
        class Wrapper
        {
            public:
                AttributeId aid;
                TypeId tid;

                Wrapper() {}
                //Type-erasure ctor, make sure t does not vanish
                template <typename T>
                    explicit Wrapper(T &t)
                    {
                        //Type erasure: we will copy obj byte for byte into data_, including its vtable
                        //Probably not very portable...
                        Object<T> obj(t);
                        const uint8_t *ptr = (uint8_t*)&obj;
                        for (auto it = data_.begin(); it != data_.end(); ++it)
                            *it = *ptr++;
                        dataIsValid_ = true;
                    }
                Wrapper(const Wrapper &rhs): aid(rhs.aid), tid(rhs.tid), data_(rhs.data_), dataIsValid_(rhs.dataIsValid_) {}
                Wrapper &operator=(const Wrapper &rhs)
                {
                    aid = rhs.aid; tid = rhs.tid; data_ = rhs.data_; dataIsValid_ = rhs.dataIsValid_;
                    return *this;
                }

                bool isValid() const {return itf_() != 0;}

                //Calls into the corresponding msgpack_set() methods of the object passed during ctor.
                void set(AttributeId aid, Nil_tag nil)       {assert(isValid()); itf_()->set(aid, nil);}
                void set(AttributeId aid, long v)            {assert(isValid()); itf_()->set(aid, v);}
                void set(AttributeId aid, const String &str) {assert(isValid()); itf_()->set(aid, str);}

            private:
                //Unity-sausage interface
                class Object_itf
                {
                    public:
                        Object_itf(): ptr_(0) {}
                        Object_itf(void *ptr): ptr_(ptr) {}

                        virtual void set(AttributeId, Nil_tag) {S();L("nil");}
                        virtual void set(AttributeId, long v) {S();L(v);}
                        virtual void set(AttributeId, const String &str) {S();L(str);}

                    protected:
                        //We will store the address of the wrapped object here
                        void *ptr_;
                };

                //We will store a _binary_ copy of an Object instance into data_, with vtable and all
                typedef gubg::array<uint8_t, sizeof(Object_itf)> Data;
                Data data_;
                bool dataIsValid_ = false;

                Object_itf *itf_() {return dataIsValid_ ? (Object_itf*)data_.data() : 0;}
                const Object_itf *itf_() const {return dataIsValid_ ? (const Object_itf*)data_.data() : 0;}

                template <typename T>
                    class Object: public Object_itf
                {
                    private:
                        typedef Object_itf Base;

                    public:
                        Object(T &t): Base(&t) {}
                        virtual void set(AttributeId aid, Nil_tag nil)       { obj_().msgpack_set(aid, nil); }
                        virtual void set(AttributeId aid, long v)            { obj_().msgpack_set(aid, v); }
                        virtual void set(AttributeId aid, const String &str) { obj_().msgpack_set(aid, str); }

                    private:
                        //Reversal of the type erasure
                        T &obj_() {return *reinterpret_cast<T*>(Base::ptr_);}
                };
        };

    //Helper funcions to wrap an object. Typically, you want to clear the object first,
    //reducing any dependency an previous garbage in it.
    template <typename String, typename T>
        Wrapper<String> wrap(T &t)
        {
            //We make sure to clear t before deserializing it
            t = T();
            return Wrapper<String>(t);
        }
    //If you know what you are doing, you can skip clearing the object
    template <typename String, typename T>
        Wrapper<String> wrapWithoutClear(T &t)
        {
            return Wrapper<String>(t);
        }

} }
#include "gubg/log/end.hpp"

#endif
