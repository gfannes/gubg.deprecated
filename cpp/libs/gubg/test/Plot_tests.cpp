#include "gubg/Plot.hpp"
#include <array>
#include <vector>
#include <thread>

int main()
{
    std::array<gubg::Plot, 2> plots;
    for (int i = 0; i < 10; ++i)
    {
        for (auto &p: plots)
        {
            std::ostringstream oss;
            oss << "sin(x+" << i*0.1 << ")";
            p.function(oss.str());
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    std::vector<double> xs{1.0, 2.0, 3.0};
    std::vector<double> ys{1.0, 3.0, 2.0};
    plots[0].scatter(xs, ys);
    std::this_thread::sleep_for(std::chrono::seconds(10));
    return 0;
}
