#include "garf/Elapser.hpp"
#include "garf/Types.hpp"
#include "garf/Blinker.hpp"
#include "gubg/msgpack/Factory.hpp"
using namespace gubg::msgpack;

typedef garf::Elapser Elapser;
Elapser g_elapser;

namespace my
{
    typedef gubg::FixedVector<char, 10> String;
}

garf::Blinker<100> g_blinker;

class Factory: public gubg::msgpack::Factory_crtp<Factory, my::String, 15>
{
    public:
        gubg::msgpack::Wrapper<my::String> msgpack_createObject(AttributeId aid, TypeId tid)
        {
            switch (tid)
            {
                case garf::TypeIds::Led:
                    return wrap(led);
            }
            return gubg::msgpack::Wrapper<my::String>();
        }
        void msgpack_createdObject(AttributeId aid, TypeId tid)
        {
            switch (tid)
            {
                case garf::TypeIds::Led:
                    if (led.id == 13)
                        g_blinker.setPattern(led.pattern);
                    break;
            }
        }
        void msgpack_set(AttributeId id, gubg::msgpack::Nil_tag) {}
        void msgpack_set(AttributeId id, const my::String &str) {}
        void msgpack_set(AttributeId id, long) {}

        garf::Led led;
};
Factory factory;

void setup()
{
    Serial.begin(9600);
    g_blinker.setPattern(0xf0);
    factory.led.id = 0;
}

void loop()
{
    g_elapser.process();
    g_blinker.process(g_elapser.elapse());
    if (Serial.available())
    {
        factory.process((unsigned char)Serial.read());
    }
}
