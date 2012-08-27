#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include "gubg/testing/Testing.hpp"
#include "gubg/tcp/Socket.hpp"

class Server: public gubg::tcp::Socket_crtp<Server, gubg::tcp::Role::Server>
{
    public:
        int socket_serverPort() const {return 1234;}
};

int main()
{
    TEST_TAG(main);
    Server server;
    server.process();
    server.process();
    server.process();
    server.process();
    server.process();
    return 0;
}
