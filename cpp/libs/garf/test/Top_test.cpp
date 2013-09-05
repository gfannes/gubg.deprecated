#include "garf/Elapser.hpp"
#include "garf/Top.hpp"

garf::Elapser g_elapser;

struct Top: garf::top::Top_crtp<Top, 1000>
{
    template <typename Msg>
        void top_msgpack(const Msg &msg)
        {
            Serial.write(msg.data(), msg.size());
        }
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
