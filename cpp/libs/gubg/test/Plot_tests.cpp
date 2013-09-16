#include "gubg/Plot.hpp"
#include "gubg/distribution/Uniform.hpp"
#include "gubg/math/Norm.hpp"
#include <array>
#include <vector>
#include <thread>
using namespace std;

int main()
{
    {
        gubg::Plot p;
        typedef array<double, 2> Coord;
        vector<Coord> coords;
        for (int i = 0; i < 10000; ++i)
        {
            using namespace gubg;
            coords.push_back(Coord({distribution::uniform(-1, 1), distribution::uniform(-1, 1)}));
        }
        p.vectorField(coords, [](const Coord &c){auto n = 30*gubg::math::l2Norm(c); return array<double, 4>({c[0], c[1], -c[1]/n, c[0]/n});});
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
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
    }
    return 0;
}
