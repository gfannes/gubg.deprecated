#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/testing/Testing.hpp"
#include "gubg/internet/Server.hpp"
#include <chrono>
#include <iostream>
using namespace std;
#define L(m) cout<<m<<endl

class Echo
{
    public:
        bool receive(const string &msg)
        {
            L("Received msg: " << msg);
            return true;
        }
};

typedef gubg::internet::Server<Echo> Server;

int main()
{
    TEST_TAG(main);
    Server server(1234);
    TEST_OK(server.start());
    this_thread::sleep_for(chrono::seconds(10));
    TEST_OK(server.stop());
    return 0;
}
