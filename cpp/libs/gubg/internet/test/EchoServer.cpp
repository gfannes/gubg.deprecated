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
            LOG_S(endpoint_receive, "Received msg: " << msg);
            send(msg);
            return true;
        }
};

typedef gubg::internet::Server<EchoEndpoint> EchoServer;

int main()
{
    TEST_TAG(main);
    EchoServer server(1234);
    TEST_OK(server.start());
    while (true)
        this_thread::sleep_for(chrono::seconds(1));
    TEST_OK(server.stop());
    return 0;
}
