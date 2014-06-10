#include "gubg/mss.hpp"
#include "gubg/msgpack/Serializer.hpp"
#include "gubg/d9/D9.hpp"
#include "gubg/testing/Testing.hpp"
#include "gubg/tty/Endpoint.hpp"
#include "gubg/Timer.hpp"
#include "garf/Types.hpp"
#include "SDL/SDL.h"
#include <iostream>
#include <thread>
using namespace gubg;
using namespace std;

#define GUBG_MODULE_ "Rover"
#include "gubg/log/begin.hpp"
enum class ReturnCode
{
    MSS_DEFAULT_CODES,
    NoJoystickFound, CouldNotOpenJoystick
};

namespace 
{
    int nrJoysticks;
    SDL_Joystick *joystick = 0;
    class Arduino: public gubg::tty::Endpoint_crtp<Arduino>
    {
        public:
            Arduino():Endpoint_crtp("/dev/ttyACM0"){}
            void endpoint_received(unsigned char ch)
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)ch << '|';
                std::cout.flush();
            }
    };
    class KeepAlive: public gubg::Timer_crtp<KeepAlive>
    {
        public:
            garf::pod::Motor motor;

            KeepAlive(Arduino &arduino):arduino_(arduino){}
            void timer_expired()
            {
                S();
                if (false)
                {
                    L("Stay awake man");
                    serializer_.clear();
                    serializer_.serialize(nullptr);
                }
                else
                {
                    serializer_.frame(motor);
                }

                std::string msg;
                gubg::d9::encode(msg, serializer_.buffer());
                L(testing::toHex(msg));
                arduino_.send(msg);

                reset();
            }
        private:
            Arduino &arduino_;
            typedef gubg::msgpack::Serializer<std::string, 5> Serializer;
            Serializer serializer_;
    };
}

ReturnCode setup()
{
    MSS_BEGIN(ReturnCode);
    MSS(SDL_Init(SDL_INIT_EVERYTHING) != -1);
    nrJoysticks = SDL_NumJoysticks();
    MSS(nrJoysticks > 0, NoJoystickFound);
    joystick = SDL_JoystickOpen(0);
    MSS(joystick, CouldNotOpenJoystick);
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

    Arduino arduino;
    KeepAlive keepAlive(arduino);
    keepAlive.setTimeout(std::chrono::milliseconds(100));

    vector<int> directions(2);

    while (!quit)
    {
        arduino.process();
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
                switch (event.jaxis.axis)
                {
                    case 0:
                        //L("X " << event.jaxis.value);
                        directions[0] = event.jaxis.value;
                        break;
                    case 1:
                        //L("Y " << event.jaxis.value);
                        directions[1] = event.jaxis.value;
                        break;
                }
                keepAlive.motor.left = (+directions[0]-directions[1])/800;
                keepAlive.motor.right = (-directions[0]-directions[1])/800;
            }
        }
    }
    MSS_END();
}
ReturnCode main_()
{
    MSS_BEGIN(ReturnCode);
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
