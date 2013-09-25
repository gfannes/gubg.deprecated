#include "gubg/bayesian/ParticleFilter.hpp"
#include "gubg/distribution/Gaussian.hpp"
#include "gubg/distribution/Estimation.hpp"
#include "gubg/Plot.hpp"
#include "gubg/Macro.hpp"
#include "gubg/math/Norm.hpp"
#include <thread>
#include <complex>
#include <iostream>
#include <fstream>
#include <array>
using namespace std;

typedef double Control;
//[0]: location
//[1]: speed
typedef array<array<double, 2>, 2> State;
typedef array<double, 2> Observation;

//The stochastic volatility model comes from http://www.ece.sunysb.edu/~zyweng/particle.html

class System
{
    public:
        typedef int Time;
        typedef ::State State;

        typedef vector<Time> TimeAry;
        typedef vector<Control> ControlAry;
        typedef vector<State> StateAry;
        typedef vector<Observation> ObservationAry;

        TimeAry timeAry;
        ControlAry controlAry;
        StateAry stateAry;
        ObservationAry observationAry;

        System():
            obs_s_(0.2), move_s_(0.1), time_(0)
    {
        state_[1] = {1.0, 1.0};
    }
        Control control() const {return control_;}
        Observation observation() const {return observation_;}
        void operator()(Control control)
        {
            ++time_;
            control_ = control;
            updateState_(state_, 0.001);
            observation_[0] = gubg::distribution::drawGaussian(state_[0][0], obs_s_);
            observation_[1] = gubg::distribution::drawGaussian(state_[0][1], obs_s_);

            timeAry.push_back(time_);
            controlAry.push_back(control_);
            stateAry.push_back(state_);
            observationAry.push_back(observation_);
        }
        void updateState(State &state, Control) const
        {
            state[1][0] = gubg::distribution::drawGaussian(0.0, 0.05);
            state[1][1] = gubg::distribution::drawGaussian(0.0, 0.05);
            updateState_(state, 0.1);
        }
        void updateState_(State &state, double f) const
        {
            state[0][0] += gubg::distribution::drawGaussian(0.01*state[1][0], f*move_s_);
            state[0][1] += gubg::distribution::drawGaussian(0.01*state[1][1], f*move_s_);
        }
        double observation_prob(Observation obs, State state) const
        {
            if (time_ > 150)
                return 1;
            return exp(-0.5*gubg::math::l2Dist2(obs, state[0])/(obs_s_*obs_s_))/obs_s_;
        }
    private:
        const double obs_s_;
        const double move_s_;
        Time time_;
        Control control_;
        State state_;
        Observation observation_;
};

int main()
{
    System s;
    typedef gubg::bayesian::ParticleFilter<System> ParticleFilter;
    ParticleFilter f(s, 1000);
    {
        const int Nr = 500;
        gubg::Plot p;
        for (int i = 0; i < Nr; ++i)
        {
            s(0.0);
            f.process(s.control(), s.observation());
            const double fact = 0.1;
            p.vectorField(f.particles(), [fact](const State &s){return array<double, 4>({s[0][0], s[0][1], fact*s[1][0], fact*s[1][1]});});
            //std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    return 0;
}