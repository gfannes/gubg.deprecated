#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/testing/Testing.hpp"
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
    };
}

int main()
{
    TEST_TAG(main);
    gubg::internet::Client client("pi", 1234);
    SpeedTest::Ptr speedTest;
    TEST_OK(client.createConnection(speedTest));
    string onek(1024, 'a');
    for (size_t i = 0; i < 10; ++i)
        TEST_OK(speedTest->send(onek));
    this_thread::sleep_for(chrono::seconds(10));
    return 0;
}
