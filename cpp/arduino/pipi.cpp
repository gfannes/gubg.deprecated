#include "gubg/Testing.hpp"
#include "gubg/internet/Socket.hpp"
#include "gubg/tty/Endpoint.hpp"
#include <iostream>
using namespace std;
#define L(m) cout<<m<<endl

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
                //L("Stay awake man");
                arduino_.send("\xd9\xc0");
                reset();
            }
        private:
            Arduino &arduino_;
    };
}

int main()
{
    Arduino arduino;
    KeepAlive keepAlive(arduino);
    keepAlive.setTimeout(std::chrono::milliseconds(500));

    gubg::internet::Socket server;
    MSS(server.bind(1234));
    MSS(server.listen());

    while (true)
    {
        gubg::internet::Socket peer;
        TEST_OK(server.accept(peer));
        gubg::internet::Socket::IOBuffer buffer(16);
        while (gubg::mss::isOK(peer.receive(buffer)))
        {
            L("Received " << buffer.str());
            while (!buffer.empty())
            {
                TEST_OK(peer.send(buffer));
            }
            buffer.clear();
        }
    }
    return 0;
}
