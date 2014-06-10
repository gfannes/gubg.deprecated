#include "gubg/Testing.hpp"
#include "gubg/internet/Client.hpp"
#include "gubg/internet/Endpoint.hpp"
#include "gubg/tty/Endpoint.hpp"
#include "gubg/Timer.hpp"
#include "gubg/msgpack/Serializer.hpp"
#include "gubg/d9/D9.hpp"
#include "gubg/OnlyOnce.hpp"
#include "garf/Types.hpp"
#include "SDL/SDL.h"
#include <thread>
#include <chrono>
#include <string>
using namespace gubg;
using namespace std;

#define GUBG_MODULE "croco"
#include "gubg/log/begin.hpp"
enum class ReturnCode {MSS_DEFAULT_CODES, NoJoystickFound, CouldNotOpenJoystick};

namespace 
{
    class PiPi: public gubg::internet::Endpoint_crtp<PiPi>
    {
        public:
            PiPi(gubg::internet::Socket socket):
                Endpoint_crtp(socket) { }
            bool endpoint_receive(const string &msg)
            {
                S();L("Received " << msg.size() << " bytes");
                return true;
            }
            bool endpoint_closed()
            {
                S();L("Connection was closed");
                return true;
            }
    };
    PiPi::Ptr pipi;

    int nrJoysticks;
    SDL_Joystick *joystick = 0;

    class KeepAlive: public gubg::Timer_crtp<KeepAlive>
    {
        public:
            ReturnCode timer_expired()
            {
                MSS_BEGIN(ReturnCode);
                //Make sure reset() is always called
                reset();
                MSS(sendMessage_());
                MSS_END();
            }

            void set(const garf::pod::Motor &motor)
            {
                motor_ = motor;
                sendMotor_.reset();
            }

        private:
            ReturnCode sendMessage_()
            {
                MSS_BEGIN(ReturnCode);

                if (!sendMotor_())
                {
                    pipi->send("\xd9\x80\xc0");
                    MSS_RETURN_OK();
                }

                MSS(serializer_.frame(motor_));
                std::string msg;
                gubg::d9::encode(msg, serializer_.buffer());
                L(testing::toHex(msg));
                pipi->send(msg);

                MSS_END();
            }

            typedef gubg::msgpack::Serializer<std::string, 10> Serializer;
            Serializer serializer_;
            gubg::OnlyOnce sendMotor_;
            garf::pod::Motor motor_;
    };
}

ReturnCode setup()
{
    MSS_BEGIN(ReturnCode, setup);

    MSS(SDL_Init(SDL_INIT_EVERYTHING) != -1);
    nrJoysticks = SDL_NumJoysticks();
    MSS(nrJoysticks > 0, NoJoystickFound);
    joystick = SDL_JoystickOpen(0);
    MSS(joystick, CouldNotOpenJoystick);

    //string peerIP = "localhost";
    //string peerIP = "192.168.0.103";
    string peerIP = "192.168.0.100";
    gubg::internet::Client client(peerIP, 1234);
    MSS(client.createConnection(pipi));

    MSS_END();
}
void teardown()
{
    if (joystick)
        SDL_JoystickClose(joystick);
    SDL_Quit();
}
ReturnCode poll()
{
    MSS_BEGIN(ReturnCode);
    bool quit = false;

    KeepAlive keepAlive;
    keepAlive.setTimeout(std::chrono::milliseconds(100));

    vector<int> directions(2);

    while (!quit)
    {
        //S();
        keepAlive.process();

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
                break;
            }

            if (event.type == SDL_JOYAXISMOTION)
            {
                S();
                switch (event.jaxis.axis)
                {
                    case 0:
                        L("X " << event.jaxis.value);
                        directions[0] = event.jaxis.value;
                        break;
                    case 1:
                        L("Y " << event.jaxis.value);
                        directions[1] = event.jaxis.value;
                        break;
                }
                garf::pod::Motor motor;
                {
                    motor.left = (-directions[0]-directions[1])/1200;
                    motor.right = (+directions[0]-directions[1])/1200;
                }
                keepAlive.set(motor);
            }
        }
    }
    MSS_END();
}
ReturnCode main_()
{
    MSS_BEGIN(ReturnCode, main_);
    MSS(setup());
    MSS(poll());
    teardown();
    MSS_END();
}

int main()
{
    return (mss::isOK(main_()) ? 0 : -1);
}
#include "gubg/log/end.hpp"
