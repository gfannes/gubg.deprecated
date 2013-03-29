#include "gubg/Plot.hpp"
#include <array>
#include <thread>

int main()
{
    std::array<gubg::Plot, 2> plots;
    for (int i = 0; i < 100; ++i)
    {
        for (auto &p: plots)
        {
            p.test(i*0.1);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    return 0;
}
