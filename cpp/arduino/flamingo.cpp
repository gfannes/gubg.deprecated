#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "garf/OOStatus.hpp"
#include "garf/BusyProcess.hpp"
#include "garf/Motor.hpp"

typedef unsigned char ubyte;

garf::Elapser g_elapser;

//This blinker is used to show the online/offline status
//We blink 10 times per second
garf::Blinker<200> g_blinker;

garf::Motor<5, 6> g_leftMotor;
garf::Motor<10, 11> g_rightMotor;

//We we don't receive indicateOnline() within 1 second, we will
//switch to offline
struct OOStatus: garf::OOStatus_crtp<OOStatus, 1000>
{
    void oostatus_online()
    {
        g_blinker.set(garf::BlinkMode::Normal);
    }
    void oostatus_offline()
    {
        g_leftMotor.setSpeed(0);
        g_rightMotor.setSpeed(0);
        g_blinker.set(garf::BlinkMode::Fast);
    }
};
OOStatus g_oostatus;

#include "gubg/msgpack/Parser.hpp"
#include "gubg/d9/Decoder.hpp"
#include "gubg/FixedVector.hpp"
using namespace gubg;

typedef gubg::FixedVector<gubg::msgpack::Element, 2> Path;
struct Parser: gubg::msgpack::Parser_crtp<Parser, Path>
{
    typedef gubg::msgpack::ReturnCode ReturnCode;
    typedef gubg::msgpack::Element Element;
    typedef gubg::msgpack::Nil_tag Nil_tag;

    int motorValues_[2];
    bool freshValues;

    Parser():freshValues(false){}

    ReturnCode parser_open(Element &element, const Path &path)
    {
        MSS_BEGIN(ReturnCode);
        MSS(path.empty());
        MSS(element.length == 2);
        MSS_END();
    }
    ReturnCode parser_close(Element &element, const Path &path)
    {
        MSS_BEGIN(ReturnCode);
        MSS(path.empty());
        MSS(element.length == 2);
        freshValues = true;
        g_oostatus.indicateOnline();
        MSS_END();
    }
    ReturnCode parser_add(unsigned long &ul, const Path &path)
    {
        MSS_BEGIN(ReturnCode);
        MSS(path.size() == 1);
        auto &el = path.back();
        MSS(el.length == 2);
        MSS(el.ix < 2);
        motorValues_[el.ix] = ul;
        MSS_END();
    }
    ReturnCode parser_add(long &l, const Path &path)
    {
        MSS_BEGIN(ReturnCode);
        MSS(path.size() == 1);
        auto &el = path.back();
        MSS(el.length == 2);
        MSS(el.ix < 2);
        motorValues_[el.ix] = l;
        MSS_END();
    }
    ReturnCode parser_add(Nil_tag, const Path &path)
    {
        //Nil => keep alive
        MSS_BEGIN(ReturnCode);
        MSS(path.empty());
        g_oostatus.indicateOnline();
        Serial.print("\xd9\xc0");
        MSS_END();
    }
};
Parser g_parser;

typedef gubg::FixedVector<ubyte, 8> Flips;
struct Decoder: gubg::d9::Decoder_crtp<Decoder, Flips>
{
    enum State {Idle, Receiving, Received};
    State state;

    Decoder():state(Idle){}

    d9::ReturnCode d9_start()
    {
        MSS_BEGIN(d9::ReturnCode);
        state = Receiving;
        g_parser.clear();
        MSS_END();
    }
    d9::ReturnCode d9_error(d9::ReturnCode)
    {
        MSS_BEGIN(d9::ReturnCode);
        state = Idle;
        MSS_END();
    }
    d9::ReturnCode d9_ubyte(ubyte b)
    {
        MSS_BEGIN(d9::ReturnCode);
        MSS(state == Receiving);
        switch (g_parser.process(b))
        {
            case msgpack::ReturnCode::OK:
                break;
            case msgpack::ReturnCode::ParsingFinished:
                state = Received;
                MSS_LQ(ContentComplete);
                break;
            default:
                state = Idle;
                MSS_L(IllegalArgument);
                break;
        }
        MSS_END();
    }
};
Decoder g_decoder;

long i;
void setup()
{
    g_blinker.set(garf::BlinkMode::Flat);
    garf::busyProcess<1000>(g_blinker);
    Serial.begin(9600);
    g_oostatus.setup();
    i = 0;
}

void loop()
{
    ++i;
    g_elapser.process();
    g_blinker.process(g_elapser.elapse());
    g_oostatus.process(g_elapser.elapse());

    if (Serial.available())
    {
        if (!mss::isOK(g_decoder.process(Serial.read())))
        {
            g_decoder.reset();
        }
    }

    if (g_parser.freshValues)
    {
        g_parser.freshValues = false;
        g_leftMotor.setSpeed(4*g_parser.motorValues_[0]);
        g_rightMotor.setSpeed(4*g_parser.motorValues_[1]);
    }
}
