#include "gubg/Testing.hpp"
#include "gubg/internet/Socket.hpp"
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    gubg::internet::Socket server;
    TEST_OK(server.bind(1234));
    TEST_KO(server.bind(1234));
    TEST_OK(server.listen());
    TEST_KO(server.listen());

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
#include "gubg/log/end.hpp"
