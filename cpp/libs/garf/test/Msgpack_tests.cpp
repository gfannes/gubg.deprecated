#include "garf/Elapser.hpp"
#include "gubg/msgpack/Factory.hpp"
using namespace gubg::msgpack;

typedef garf::Elapser Elapser;
Elapser g_elapser;

namespace my
{
    typedef gubg::FixedVector<char, 10> String;
}

class Data
{
    public:
        void msgpack_set(AttributeId id, gubg::msgpack::Nil_tag) {}
        void msgpack_set(AttributeId id, const my::String &str) {str_ = str;}
        void msgpack_set(AttributeId id, long) {}
    private:
        my::String str_;
};
Data data;
class Factory: public gubg::msgpack::Factory_crtp<Factory, my::String, 15>
{
    public:
        gubg::msgpack::Wrapper<my::String> msgpack_createObject(AttributeId aid, TypeId tid)
        {
            switch (tid)
            {
                case 0: return wrap(data);
            }
            return gubg::msgpack::Wrapper<my::String>();
        }
        void msgpack_createdObject(AttributeId aid, TypeId tid)
        {
        }
        void msgpack_set(AttributeId id, gubg::msgpack::Nil_tag) {}
        void msgpack_set(AttributeId id, const my::String &str) {}
        void msgpack_set(AttributeId id, long) {}
};
Factory factory;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    g_elapser.process();
    if (Serial.available())
    {
        factory.process((unsigned char)Serial.read());
    }
}
