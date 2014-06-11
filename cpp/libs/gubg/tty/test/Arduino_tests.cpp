#include "gubg/tty/Endpoint.hpp"
#include "gubg/Timer.hpp"
#include <iostream>
#include <iomanip>

namespace 
{
    class Arduino: public gubg::tty::Endpoint_crtp<Arduino>
    {
        public:
            Arduino():
                i(0),
                Endpoint_crtp("/dev/ttyACM0"){}
            void endpoint_received(unsigned char ch)
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)ch << '|';
                std::cout.flush();
            }
        private:
            int i;
    };
    class Timer: public gubg::Timer_crtp<Timer>
    {
        public:
            Timer(Arduino &arduino):arduino_(arduino){}
            void timer_expired()
            {
                reset();
                std::cout << "Sending message" << std::endl;
                std::cout.flush();
                arduino_.send("\xd9\x80\xc0");
            }
        private:
            Arduino &arduino_;
    };
}

int main()
{
    Arduino a;
    Timer timer(a);
    timer.setTimeout(std::chrono::milliseconds(200));
    for (;;)
    {
        a.process();
        timer.process();
    }
    return 0;
}
