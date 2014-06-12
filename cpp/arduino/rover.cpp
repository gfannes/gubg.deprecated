#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "garf/OOStatus.hpp"
#include "garf/BusyProcess.hpp"
#include "garf/Motor.hpp"
#include "garf/Types.hpp"
#include "garf/iostream.hpp"

#include "gubg/msgpack/Serializer.hpp"
#include "gubg/d9/D9.hpp"

using garf::cout;
using garf::endl;

typedef unsigned char ubyte;

garf::Elapser g_elapser;

//This blinker is used to show the online/offline status
//We blink 10 times per second
garf::Blinker<100> g_blinker;

garf::Motor<5, 6> g_leftMotor;
garf::Motor<9, 10> g_rightMotor;

//We we don't receive indicateOnline() within 1 second, we will
//switch to offline
struct OOStatus: garf::OOStatus_crtp<OOStatus, 1000>
{
    void oostatus_online()
    {
        //garf::cout << "online" << garf::endl;
        g_blinker.setPattern(0xf0);
    }
    void oostatus_offline()
    {
        //garf::cout << "offline" << garf::endl;
        g_leftMotor.setSpeed(0);
        g_rightMotor.setSpeed(0);
        g_blinker.setPattern(0xee);
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

        void msgpack_beginOfFrame()
        {
            //cout << "begin of frame" << endl;
        }
        void msgpack_endOfFrame() {}
        template <typename Wrapper>
            void msgpack_createObject(Wrapper &obj, RoleId rid)
            {
                switch (rid)
                {
                    case garf::pod::TypeIds::Motor: obj = wrap(motor); break;
                }
            }
        void msgpack_createdObject(RoleId rid)
        {
            g_oostatus.indicateOnline();
            dirty = true;
            switch (rid)
            {
                case garf::pod::TypeIds::Motor:
                    //garf::cout << motor << garf::endl;
                    break;
            }
        }
        void msgpack_set(RoleId id, gubg::msgpack::Nil_tag)
        {
            g_oostatus.indicateOnline();
        }
        void msgpack_set(RoleId id, const my::String &str) {}
        void msgpack_set(RoleId id, long) {}

    private:
};
Factory g_factory;

gubg::FixedVector<uint8_t, 4> d9Header_;
gubg::msgpack::Serializer<gubg::FixedVector<uint8_t, 20>, 3> serializer_;
void sendAnswer()
{
    serializer_.frame(g_factory.motor);
    gubg::d9::encodeInPlace(d9Header_, serializer_.buffer());
    Serial.write(d9Header_.data(), d9Header_.size());
    Serial.write(serializer_.buffer().data(), serializer_.buffer().size());
}

typedef gubg::FixedVector<ubyte, 8> Flips;
class Decoder: public gubg::d9::Decoder_crtp<Decoder, Flips>
{
    public:
        void d9_start()
        {
            ok_ = true;
            g_factory.reset();
        }
        void d9_error(d9::ReturnCode) {ok_ = false;}
        void d9_ubyte(ubyte b)
        {
            if (b == 0xc0)
                g_oostatus.indicateOnline();
            if (ok_)
                g_factory.process(b);
        }
    private:
        bool ok_ = false;
};
Decoder g_decoder;

void setup()
{
    Serial.begin(9600);

    //Indicate boot cycle
    g_blinker.setPattern(0xaa);
    while (millis() < 2000)
    {
        g_elapser.process();
        g_blinker.process(g_elapser.elapse());
    }
    g_oostatus.setup();
}

void loop()
{
    g_elapser.process();
    g_blinker.process(g_elapser.elapse());
    g_oostatus.process(g_elapser.elapse());
    g_leftMotor.process(g_elapser.elapse());
    g_rightMotor.process(g_elapser.elapse());

#if 0
    while (Serial.available())
    {
        g_oostatus.indicateOnline();
        const auto ch = Serial.read();
        Serial.print("Received: ");
        Serial.println(ch);
    }
#endif

#if 1
    if (Serial.available())
    {
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
        //sendAnswer();
    }
#endif
}
