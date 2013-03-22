#include "gubg/Testing.hpp"
#include "gubg/internet/Socket.hpp"
#include "gubg/tty/Endpoint.hpp"
#include <iostream>
using namespace gubg;
using namespace std;

enum class ReturnCode {MSS_DEFAULT_CODES, };

namespace 
{
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
}

ReturnCode main_()
{
    MSS_BEGIN(ReturnCode, main_);
    Arduino arduino;

    gubg::internet::Socket server;
    MSS(server.bind(1234));
    MSS(server.listen());

    while (true)
    {
        gubg::internet::Socket peer;
        MSS(server.accept(peer));
        gubg::internet::Socket::IOBuffer buffer(16);
        while (gubg::mss::isOK(peer.receive(buffer)))
        {
            arduino.process();
            L("Received " << testing::toHex(buffer.str()));
            while (!buffer.empty())
            {
                L("Sending " << testing::toHex(buffer.str()));
                arduino.send(buffer.str());
                buffer.clear();
            }
            buffer.clear();
        }
    }
    MSS_END();
}
int main()
{
    return (MSS_IS_OK(main_()) ? 0 : -1);
}
