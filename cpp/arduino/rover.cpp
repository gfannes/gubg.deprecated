#include "garf/Elapser.hpp"
#include "garf/Blinker.hpp"
#include "garf/OOStatus.hpp"
#include "garf/BusyProcess.hpp"
#include "garf/Motor.hpp"
#include "garf/Types.hpp"

#include "gubg/msgpack/Serializer.hpp"
#include "gubg/d9/D9.hpp"

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

        gubg::msgpack::Wrapper<my::String> msgpack_createObject(AttributeId aid, TypeId tid)
        {
            switch (tid)
            {
                case garf::pod::TypeIds::Motor:
                    return wrapWithoutClear(motor);
            }
            return gubg::msgpack::Wrapper<my::String>();
        }
        void msgpack_createdObject(AttributeId aid, TypeId tid)
        {
            g_oostatus.indicateOnline();
            dirty = true;
            switch (tid)
            {
                case garf::pod::TypeIds::Motor:
                    break;
            }
        }
        void msgpack_set(AttributeId id, gubg::msgpack::Nil_tag)
        {
            g_oostatus.indicateOnline();
        }
        void msgpack_set(AttributeId id, const my::String &str) {}
        void msgpack_set(AttributeId id, long) {}

    private:
};
Factory g_factory;

gubg::FixedVector<uint8_t, 4> d9Header_;
gubg::msgpack::Serializer<gubg::FixedVector<uint8_t, 20>, garf::pod::TypeIds, 2> serializer_;
void sendAnswer()
{
    serializer_.clear();
    serializer_.serialize(g_factory.motor);
    gubg::d9::encodeInPlace(d9Header_, serializer_.buffer());
    Serial.write(d9Header_.data(), d9Header_.size());
    Serial.write(serializer_.buffer().data(), serializer_.buffer().size());
}

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

void setup()
{
    Serial.begin(9600);
    g_oostatus.setup();
}

void loop()
{
    g_elapser.process();
    g_blinker.process(g_elapser.elapse());
    g_oostatus.process(g_elapser.elapse());
    g_leftMotor.process(g_elapser.elapse());
    g_rightMotor.process(g_elapser.elapse());

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
}
