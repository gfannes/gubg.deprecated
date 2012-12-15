#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/Testing.hpp"
#include "gubg/tcp/Socket.hpp"
#include <thread>
#include <chrono>
#include <iostream>
using namespace std;
#define L(m) cout<<m<<endl

class Server: public gubg::tcp::Socket_crtp<Server, gubg::tcp::Role::Server>
{
    public:
        int socket_serverPort() const {return 1234;}
        void socket_newClient(int fid){L("A new client was accepted: " << fid);}
        void socket_closeClient(int fid){L("A client was closed: " << fid);}
        void socket_ReceivedData(const string &data){L("I received data: " << data);}
};

int main()
{
    TEST_TAG(main);
    Server server;
    for (;;)
    {
        server.process();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    return 0;
}