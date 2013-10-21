#ifndef HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED
#define HEADER_gubg_msgpack_Deserialize_hpp_ALREADY_INCLUDED

#include "gubg/msgpack/Types.hpp"
#include "gubg/msgpack/Primitives.hpp"
#include "gubg/array.hpp"

#define GUBG_MODULE_ "Deserialize"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace msgpack
    {
        template <typename String>
            class Wrapper
            {
                public:
                    AttributeId aid;
                    TypeId tid;

                    Wrapper() {}
                    template <typename T>
                        explicit Wrapper(T &t)
                    {
                        Object<T> obj(t);
                        const uint8_t *ptr = (uint8_t*)&obj;
                        for (auto it = data_.begin(); it != data_.end(); ++it)
                            *it = *ptr++;
                    }
                    Wrapper(const Wrapper &rhs): data_(rhs.data_) {}
                    Wrapper &operator=(const Wrapper &rhs) {data_ = rhs.data_;}

                    bool isValid() const {return itf_() != 0;}

                    void set(AttributeId aid, Nil_tag nil)       {itf_()->set(aid, nil);}
                    void set(AttributeId aid, long v)            {itf_()->set(aid, v);}
                    void set(AttributeId aid, const String &str) {itf_()->set(aid, str);}

                private:
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

                    gubg::array<uint8_t, sizeof(Object_itf)> data_;
                    //Returns 0 for a default-constructed object
                    Object_itf *itf_() {return (Object_itf*)data_.data();}
                    const Object_itf *itf_() const {return (const Object_itf*)data_.data();}

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
                            T &obj_() {return *reinterpret_cast<T*>(Base::ptr_);}
                    };
            };

        template <typename String, typename T>
            Wrapper<String> wrap(T &t)
            {
                //We make sure to clear t before deserializing it
                t = T();
                return Wrapper<String>(t);
            }
        template <typename String, typename T>
            Wrapper<String> wrapWithoutClear(T &t)
            {
                return Wrapper<String>(t);
            }
    }
}
#include "gubg/log/end.hpp"

#endif
