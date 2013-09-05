#include "garf/Elapser.hpp"
#include "garf/Metronome.hpp"
#include "garf/Time.hpp"
#include "garf/Types.hpp"
#include "gubg/msgpack/Serializer.hpp"

typedef garf::Elapser Elapser;
Elapser g_elapser;

class SendTime: public garf::Metronome_crtp<SendTime, 1000>
{
    public:
        void metronome_tick()
        {
            serializer_.clear();
            serializer_.serialize(garf::time::now());
            Serial.write(serializer_.buffer().data(), serializer_.buffer().size());
        }
    private:
        gubg::msgpack::Serializer<gubg::FixedVector<uint8_t, 20>, garf::TypeIds, 2> serializer_;
};
SendTime g_sendTime;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    g_elapser.process();
    g_sendTime.process(g_elapser.elapse());
}
