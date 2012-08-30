#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/testing/Testing.hpp"
#include "gubg/internet/Server.hpp"
#include "gubg/internet/Endpoint.hpp"
#include <chrono>
#include <iostream>
using namespace std;
#define L(m) cout<<m<<endl

class EchoEndpoint: public gubg::internet::Endpoint_crtp<EchoEndpoint>
{
    public:
        EchoEndpoint(gubg::internet::Socket socket):
            Endpoint_crtp(socket){}

        bool endpoint_receive(const string &msg)
        {
            L("Received msg: " << msg);
            //We echo back 4 times whatever we receive
            for (auto i = 0; i < 4; ++i)
                send(msg);
            return true;
        }
};

typedef gubg::internet::Server<EchoEndpoint> Server;

int main()
{
    TEST_TAG(main);
    Server server(1234);
    TEST_OK(server.start());
    this_thread::sleep_for(chrono::seconds(100));
    TEST_OK(server.stop());
    return 0;
}
