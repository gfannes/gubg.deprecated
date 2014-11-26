#include "gubg/math/stat/Gaussian.hpp"
#include "gubg/Plot.hpp"
#include <thread>

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
namespace 
{
    typedef gubg::math::stat::GaussianUV<double> GaussianUV;
    typedef gubg::math::stat::GaussianMV<double, 3> GaussianMV;
    template <typename T, size_t D>
        std::string to_s(const std::array<T, D> &vs)
        {
            std::ostringstream oss;
            for (auto v: vs)
                oss << v << ", ";
            return oss.str();
        }
#define A2S(v) to_s(v)
}
int main()
{
    S();
    const size_t Nr = 10000;
    {
        GaussianUV g;
        GaussianUV::value_type v;
        for (int i = 0; i < Nr; ++i)
            g.generate(v);
        L(v);
        std::vector<double> vs(Nr);
        gubg::Plot p;
        for (int i = 0; i < 100; ++i)
        {
            g.generate(vs);
            p.histogram(vs, 200);
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    {
        GaussianMV g;
        g.setSigma({1.0, 0.1, 10.0});
        GaussianMV::value_type v;
        for (int i = 0; i < Nr; ++i)
            g.generate(v);
        L(A2S(v));
        std::vector<GaussianMV::value_type> vs(Nr);
        gubg::Plot p;
        for (int i = 0; i < 100; ++i)
        {
            g.generate(vs);
            p.scatter(vs, 0, 2);
            //std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    return 0;
}
#include "gubg/log/end.hpp"
