#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "garf/OOStatus.hpp"
#include "garf/BusyProcess.hpp"
#include "garf/Motor.hpp"
#include "garf/Average.hpp"
#include "garf/Types.hpp"

typedef unsigned char ubyte;

typedef garf::Elapser Elapser;
Elapser g_elapser;
//garf::Sonar   g_sonar;

//This blinker is used to show the online/offline status
//We blink 10 times per second
typedef garf::Blinker<100> Blinker;
Blinker g_blinker;

garf::Motor<6, 5> g_leftMotor;
garf::Motor<10, 9> g_rightMotor;

garf::RobustAverage<ubyte, 10> sonarAverage;

//If we don't receive indicateOnline() within 1 second, we go offline
struct OOStatus: garf::OOStatus_crtp<OOStatus, 1000>
{
    void oostatus_online()
    {
        g_blinker.setPattern(0xf0);
    }
    void oostatus_offline()
    {
        g_leftMotor.setSpeed(0);
        g_rightMotor.setSpeed(0);
        g_blinker.setPattern(0xaa);
    }
};
OOStatus g_oostatus;

#include "gubg/msgpack/Factory.hpp"
#include "gubg/d9/Decoder.hpp"
#include "gubg/FixedVector.hpp"
using namespace gubg;

namespace my
{
    typedef gubg::FixedVector<char, 15> String;
}
class Factory: public gubg::msgpack::Factory_crtp<Factory, my::String, 5>
{
    public:
        typedef gubg::msgpack::ReturnCode ReturnCode;

        garf::pod::Motor motor;
        bool dirty = false;

        template <typename Wrapper>
        void msgpack_createObject(Wrapper &obj, RoleId rid)
        {
            switch (rid)
            {
                case garf::pod::TypeIds::Motor: obj = wrapWithoutClear(motor); break;
            }
        }
        void msgpack_createdObject(RoleId rid)
        {
            g_oostatus.indicateOnline();
            dirty = true;
            switch (rid)
            {
                case garf::pod::TypeIds::Motor: break;
            }
        }
        void msgpack_set(RoleId id, gubg::msgpack::Nil_tag)
        {
            g_oostatus.indicateOnline();
        }
        void msgpack_set(RoleId id, const my::String &str) {}
        void msgpack_set(RoleId id, long) {g_oostatus.indicateOnline();}

    private:
};
Factory g_factory;

typedef gubg::FixedVector<ubyte, 8> Flips;
class Decoder: public gubg::d9::Decoder_crtp<Decoder, Flips>
{
    public:
        void d9_start() {ok_ = true; g_factory.reset();}
        void d9_error(d9::ReturnCode) {ok_ = false;}
        void d9_ubyte(ubyte b)
        {
            if (ok_)
                g_factory.process(b);
        }
    private:
        bool ok_ = false;
};
Decoder g_decoder;

const int trigger = 7;
const int echo = 8;
const int MaxDistance = 200;

void setup()
{
    g_elapser = Elapser();
    g_blinker.setPattern(0x01);
    garf::busyProcess<1000>(g_blinker);
    Serial.begin(9600);
    g_oostatus.setup();
}

void loop()
{
    g_leftMotor.setSpeed(0);
    g_elapser.process();
    g_blinker.process(g_elapser.elapse());
    g_oostatus.process(g_elapser.elapse());
//    g_sonar.process(g_elapser.elapse());

    if (Serial.available())
    {
        g_oostatus.indicateOnline();
        if (!mss::isOK(g_decoder.process(Serial.read())))
        {
            g_decoder.reset();
        }
    }

    if (g_factory.dirty)
    {
        g_factory.dirty = false;
        g_leftMotor.setSpeed(4*g_factory.motor.left);
        g_rightMotor.setSpeed(4*g_factory.motor.right);
    }
}
