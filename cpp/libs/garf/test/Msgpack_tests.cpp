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
        template <typename Wrapper>
            void msgpack_createObject(Wrapper &obj, RoleId rid)
            {
                switch (rid)
                {
                    case garf::pod::TypeIds::Led: obj = wrap(led); break;
                }
            }
        void msgpack_createdObject(RoleId rid)
        {
            switch (rid)
            {
                case garf::pod::TypeIds::Led:
                    if (led.id == 13)
                        g_blinker.setPattern(led.pattern);
                    break;
            }
        }
        void msgpack_set(RoleId id, gubg::msgpack::Nil_tag) {}
        void msgpack_set(RoleId id, const my::String &str) {}
        void msgpack_set(RoleId id, long) {}

        garf::pod::Led led;
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
