#include "gubg/trace/Scope.hpp"
using namespace gubg::trace;

int main()
{
    {
    Scope s1(Msg(0) << "main" << 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    Scope s2(Msg(0) << "main" << 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    return 0;
}
