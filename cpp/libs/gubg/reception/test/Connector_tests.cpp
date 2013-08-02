#include "gubg/reception/Connector.hpp"
#include <chrono>
using namespace gubg::reception;

int main()
{
    Connector connector;
    connector.watch(80);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return 0;
}
