#include "gubg/mss.hpp"
#include "gubg/msgpack/Write.hpp"
#include "gubg/testing/Testing.hpp"
#include "SDL/SDL.h"
#include <iostream>
#include <fstream>
using namespace gubg;
using namespace std;
#define L(m) cout<<m<<endl

enum class ReturnCode {MSS_DEFAULT_CODES, NoJoystickFound, CouldNotOpenJoystick};

namespace 
{
    int nrJoysticks;
    SDL_Joystick *joystick = 0;
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

    ofstream of("/dev/ttyACM0");

    vector<int> directions(2);
    vector<int> motors(2);

    while (!quit)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            else if (event.type == SDL_JOYAXISMOTION)
            {
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
                motors[0] = (-directions[0]-directions[1])/256;
                motors[1] = (+directions[0]-directions[1])/256;
                string motors_msgpack;
                msgpack::write(motors_msgpack, motors);
                ostringstream oss;
                oss << "\xd9" << motors_msgpack;
                cout << testing::toHex(oss.str()) << endl;
                of << oss.str();;
                of.flush();
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
