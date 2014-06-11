#include "gubg/OnlyOnce.hpp"
#include "gubg/mss.hpp"
#include "SDL/SDL.h"

#define GUBG_MODULE "Joystick"
#include "gubg/log/begin.hpp"
namespace gubg { namespace media { 

    enum class ReturnCode {MSS_DEFAULT_CODES, NoJoystickFound, CouldNotOpenJoystick, Quit, };

    template <typename Receiver>
        class Joystick_crtp
        {
            public:
                ~Joystick_crtp()
                {
                    if (doInit_())
                        return;
                    if (joystick)
                        SDL_JoystickClose(joystick);
                    joystick = 0;
                    SDL_Quit();
                }

                ReturnCode process()
                {
                    MSS_BEGIN(ReturnCode);

                    MSS(initialize_());

                    bool moved = false;
                    {
                        SDL_Event event;
                        while (SDL_PollEvent(&event))
                        {
                            MSS(event.type != SDL_QUIT, Quit);

                            if (event.type != SDL_JOYAXISMOTION)
                                continue;

                            switch (event.jaxis.axis)
                            {
                                case 0: x_ = event.jaxis.value; moved = true; break;
                                case 1: y_ = event.jaxis.value; moved = true; break;
                            }
                        }
                    }
                    if (moved)
                        receiver_().joystick_moved(x_, y_);

                    MSS_END();
                }

            private:
                Receiver &receiver_() {return static_cast<Receiver&>(*this);}

                ReturnCode initialize_()
                {
                    MSS_BEGIN(ReturnCode);
                    if (!doInit_())
                        MSS_RETURN_OK();
                    MSS(SDL_Init(SDL_INIT_EVERYTHING) != -1);
                    auto nrJoysticks = SDL_NumJoysticks();
                    MSS(nrJoysticks > 0, NoJoystickFound);
                    joystick = SDL_JoystickOpen(0);
                    MSS(joystick, CouldNotOpenJoystick);
                    MSS_END();
                }

                gubg::OnlyOnce doInit_;
                SDL_Joystick *joystick = 0;
                int x_ = 0;
                int y_ = 0;
        };

} } 
#include "gubg/log/end.hpp"
