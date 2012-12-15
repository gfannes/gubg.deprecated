#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/Testing.hpp"
#include "gubg/internet/Client.hpp"
#include "gubg/internet/Endpoint.hpp"
#include <thread>
#include <chrono>
#include <string>
using namespace std;

namespace 
{
    class SpeedTest: public gubg::internet::Endpoint_crtp<SpeedTest>
    {
        public:
            SpeedTest(gubg::internet::Socket socket):
                Endpoint_crtp(socket) { }
            bool endpoint_receive(const string &msg)
            {
                LOG_S(endpoint_receive, "Received " << msg.size() << " bytes");
                return true;
            }
            bool endpoint_closed()
            {
                LOG_S(endpoint_closed, "Connection was closed");
                return true;
            }
    };
    typedef gubg::internet::ReturnCode ReturnCode;
}

ReturnCode main_()
{
    MSS_BEGIN(ReturnCode);
    string peer = "piw";
    gubg::internet::Client client(peer, 1234);
    SpeedTest::Ptr speedTest;
    MSS(client.createConnection(speedTest));
    const string onek(1024, 'a');
    while (true)
    {
        for (size_t i = 0; i < 10; ++i)
            MSS(speedTest->send(onek));
        this_thread::sleep_for(chrono::seconds(1));
    }
    MSS_END();
}

int main()
{
    TEST_TAG(main);
    if (!gubg::mss::isOK(main_()))
        return -1;
    return 0;
}