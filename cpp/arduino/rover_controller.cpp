#include "gubg/mss.hpp"
#include "gubg/msgpack/Write.hpp"
#include "gubg/testing/Testing.hpp"
#include "gubg/tty/Endpoint.hpp"
#include "gubg/Timer.hpp"
#include "gubg/l.hpp"
#include "SDL/SDL.h"
#include <iostream>
using namespace gubg;
using namespace std;

enum class ReturnCode {MSS_DEFAULT_CODES, NoJoystickFound, CouldNotOpenJoystick};

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
            KeepAlive(Arduino &arduino):arduino_(arduino){}
            void timer_expired()
            {
                arduino_.send("\xd9\xc0");
                reset();
            }
        private:
            Arduino &arduino_;
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
    keepAlive.setTimeout(std::chrono::milliseconds(500));

    vector<int> directions(2);
    vector<int> motors(2);

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
                motors[0] = (-directions[0]-directions[1])/256;
                motors[1] = (+directions[0]-directions[1])/256;
                string motors_msgpack;
                msgpack::write(motors_msgpack, motors);
                ostringstream oss;
                oss << "\xd9" << motors_msgpack;
                //L(testing::toHex(oss.str()));
                arduino.send(oss.str());
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
