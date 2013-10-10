#include "gubg/Plot.hpp"
#include "gubg/distribution/Uniform.hpp"
#include "gubg/math/Norm.hpp"
#include <array>
#include <vector>
#include <complex>
#include <thread>
using namespace std;

int main()
{
    {
        gubg::Plot p;
        typedef complex<double> Polar;
        vector<Polar> polars;
        for (int i = 0; i < 10000; ++i)
        {
            using namespace gubg;
            polars.push_back(polar(distribution::drawUniform(0, 1), distribution::drawUniform(0, 3.1415926)));
        }
        p.polar(polars, [](const Polar &pp){return array<double, 2>({arg(pp), abs(pp)});});
        this_thread::sleep_for(chrono::seconds(10));
    }
    {
        gubg::Plot p;
        typedef array<double, 2> Coord;
        vector<Coord> coords;
        for (int i = 0; i < 10000; ++i)
        {
            using namespace gubg;
            coords.push_back(Coord({distribution::drawUniform(-1, 1), distribution::drawUniform(-1, 1)}));
        }
        p.vectorField(coords, [](const Coord &c){auto n = 30*gubg::math::l2::norm(c); return array<double, 4>({c[0], c[1], -c[1]/n, c[0]/n});});
        this_thread::sleep_for(chrono::seconds(10));
    }
    {
        array<gubg::Plot, 2> plots;
        for (int i = 0; i < 10; ++i)
        {
            for (auto &p: plots)
            {
                ostringstream oss;
                oss << "sin(x+" << i*0.1 << ")";
                p.function(oss.str());
                this_thread::sleep_for(chrono::milliseconds(10));
            }
        }
        vector<double> xs{1.0, 2.0, 3.0};
        vector<double> ys{1.0, 3.0, 2.0};
        plots[0].scatter(xs, ys);
        this_thread::sleep_for(chrono::seconds(10));
    }
    return 0;
}
