#include "garf/Elapser.hpp"
#include "garf/Top.hpp"
#include "gubg/d9/D9.hpp"

garf::Elapser g_elapser;

struct Top: garf::top::Top_crtp<Top, 1000>
{
    template <typename Msg>
        void top_msgpack(Msg &msg)
        {
            gubg::d9::encodeInPlace(d9Header, msg);
            Serial.write(d9Header.data(), d9Header.size());
            Serial.write(msg.data(), msg.size());
        }
    gubg::FixedVector<unsigned char, 4> d9Header;
};
Top g_top;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    g_elapser.process();
    g_top.process(g_elapser.elapse());
}
