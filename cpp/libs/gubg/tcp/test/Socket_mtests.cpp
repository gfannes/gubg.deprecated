#include "gubg/Testing.hpp"
#include "gubg/tcp/Socket.hpp"
#include <thread>
#include <chrono>
#include <string>

#define GUBG_MODULE "Server"
#include "gubg/log/begin.hpp"
class Server: public gubg::tcp::Socket_crtp<Server, gubg::tcp::Role::Server>
{
    public:
        int socket_serverPort() const {return 1234;}
        void socket_newClient(int fid){S();L("A new client was accepted: " << fid);}
        void socket_closeClient(int fid){S();L("A client was closed: " << fid);}
        void socket_ReceivedData(const std::string &data){S();L("I received data (" << data.size() << "): " << data);}
};
#include "gubg/log/end.hpp"

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
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
#include "gubg/log/end.hpp"
