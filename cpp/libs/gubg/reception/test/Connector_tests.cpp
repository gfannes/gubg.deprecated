#include "gubg/reception/Connector.hpp"
#include <chrono>
using namespace gubg::reception;

int main()
{
    Connector connector;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}
