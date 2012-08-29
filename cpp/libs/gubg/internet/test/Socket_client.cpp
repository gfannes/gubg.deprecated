#include "gubg/testing/Testing.hpp"
#include "gubg/internet/Socket.hpp"
#include <iostream>
using namespace std;
#define L(m) cout<<m<<endl

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
