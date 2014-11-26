#include "gubg/Testing.hpp"
#include "gubg/internet/Socket.hpp"
using namespace std;

#define GUBG_MODULE "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    gubg::internet::Socket socket;
    TEST_OK(socket.connect("localhost", 1234));

    gubg::internet::Socket::IOBuffer buffer(string(1024, 'a'));
    while (!buffer.empty())
    {
        TEST_OK(socket.send(buffer));
    }
    buffer.clear();
    while (!buffer.full())
    {
        TEST_OK(socket.receive(buffer));
        L("Received " << buffer.str());
    }
    return 0;
}
#include "gubg/log/end.hpp"
