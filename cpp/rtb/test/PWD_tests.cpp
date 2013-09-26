#include "rtb/PWD.hpp"
#include "gubg/Array.hpp"
#include "gubg/Plot.hpp"
#include "gubg/Testing.hpp"
#include <thread>
using rtb::pwd::State;
using rtb::pwd::Observation;
using namespace std;

namespace 
{
    double angle = 1.0;
    typedef State<3, angle> PWD;
}

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);
    PWD pwd;
    pwd.setDistances(gubg::make_array(3, 2, 1));
    pwd.setAngles(gubg::make_array(1, 3, 2));
    array<Observation, 1000> obss;
    for (auto &obs: obss)
    {
        S();
        obs = generateGaussianObservation(pwd, 0.01);
        const auto p = observation_prob(pwd, obs, 0.01);
        L(STREAM(std::abs(obs), std::arg(obs), p));
    }
    gubg::Plot plot;
    plot.polar(obss, [](Observation obs){return gubg::make_array(arg(obs), abs(obs));});
//    this_thread::sleep_for(chrono::seconds(10));
    return 0;
}
#include "gubg/log/end.hpp"
