#include "garf/Elapser.hpp"
#include "garf/Metronome.hpp"
#include "gubg/msgpack/Serializer.hpp"

typedef garf::Elapser Elapser;
Elapser g_elapser;

struct Time
{
    Time(): t_(millis()) {}
    unsigned long t_;
    template <typename S>
        gubg::msgpack::ReturnCode msgpack_serialize(S &s) const
        {
            MSS_BEGIN(gubg::msgpack::ReturnCode);
            s.writeIdAndAttrCnt(S::Time, 1);
            s.template writeAttribute<long>(0, t_);
            MSS_END();
        }
};
struct Ids
{
    enum {Time};
};
class SendTime: public garf::Metronome_crtp<SendTime, 1000>
{
    public:
        void metronome_tick()
        {
            serializer_.clear();
            serializer_.serialize(Time());
            Serial.write(serializer_.buffer().data(), serializer_.buffer().size());
        }
    private:
        gubg::msgpack::Serializer<gubg::FixedVector<uint8_t, 20>, Ids, 2> serializer_;
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
