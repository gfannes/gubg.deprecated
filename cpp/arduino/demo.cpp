#include "garf/Elapser.hpp"
#include "garf/serial/Endpoint.hpp"
#include "garf/serial/MsgpackTx.hpp"
#include "garf/Top.hpp"

garf::Elapser g_Elapser;

class Endpoint: public garf::serial::Endpoint_crtp<Endpoint>
{
    public:
        void serialendpoint_process(uint8_t b)
        {
            print("I received: ", (char)b, " ", b); newline();
        }
};
Endpoint g_Endpoint;

class MsgpackTx: public garf::serial::MsgpackTx_crtp<MsgpackTx, 5, 100>
{
    public:
        int roleId(const garf::pod::TopInfo &) const {return 123;}
};
MsgpackTx g_MsgpackTx;

class Top: public garf::top::Top_crtp<Top, 1000>
{
    public:
        void top_slice(const garf::pod::TopInfo &ti)
        {
            //g_Endpoint.print(ti.nrLoops, ' ', ti.maxElapse, "\r\n");
            g_MsgpackTx.put(ti);
        }
};
Top g_Top;

void setup() { }

void loop()
{
    g_Elapser.process();
    g_Endpoint.process();
    g_MsgpackTx.process();
    g_Top.process(g_Elapser.elapse());
}
