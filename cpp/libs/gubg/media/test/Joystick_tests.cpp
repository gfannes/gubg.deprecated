#include "gubg/media/Joystick.hpp"

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
namespace  { 
    class Joystick: public gubg::media::Joystick_crtp<Joystick>
    {
        public:
            void joystick_moved(int x, int y)
            {
                S();L(STREAM(x, y));
            }
    };
} 

bool main_()
{
    MSS_BEGIN(bool);
    Joystick j;
    while (true)
        MSS(j.process() != gubg::media::ReturnCode::Quit);
    MSS_END();
}
int main()
{
    main_();
    return 0;
}
#include "gubg/log/end.hpp"
