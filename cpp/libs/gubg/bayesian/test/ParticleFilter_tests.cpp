#include "gubg/bayesian/ParticleFilter.hpp"
#include "gubg/math/random/Normal.hpp"
#include "gubg/math/distribution/Estimation.hpp"
#include "gubg/Plot.hpp"
#include "gubg/Macro.hpp"
#include <thread>
#include <complex>
#include <iostream>
#include <fstream>
using namespace std;
using gubg::math::random::drawNormal;

typedef double Control;
typedef double State;
typedef double Observation;

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
            a_(0.91), b_(1),
            time_(0), state_(0)
    {
    }
        Control control() const {return control_;}
        Observation observation() const {return observation_;}
        void operator()(Control control)
        {
            ++time_;
            control_ = control;
            updateState(state_, control_);
            observation_ = drawNormal(0.0, b_*exp(0.5*state_));

            timeAry.push_back(time_);
            controlAry.push_back(control_);
            stateAry.push_back(state_);
            observationAry.push_back(observation_);
        }
        void updateState(State &state, Control control) const
        {
            state = a_*state + drawNormal(control, 1.0);
        }
        double observation_prob(Observation obs, State state) const
        {
            const double invSigma = 1.0/(b_*exp(0.5*state));
            double d = invSigma*(obs-0.0);
            return invSigma*exp(-0.5*d*d);
        }
        template <char D>
            void stream(ostream &os) const
            {
                os << time_ << D << control_ << D << state_ << D << observation_ << D;
            }
    private:
        const double a_;
        const double b_;
        Time time_;
        Control control_;
        State state_;
        Observation observation_;
};

    template <char D, typename Particles>
void streamParticles(ostream &os, const Particles &particles)
{
    double mean; gubg::math::distribution::computeMean(mean, particles);
    os << mean << D;
    for (auto p: particles)
        os << p << D;
}

int main()
{
    System s;
    typedef gubg::bayesian::ParticleFilter<System> ParticleFilter;
    ParticleFilter f(s, 1000, 0.0);
    {
        const int Nr = 100;
        ofstream fo("data.csv");
        for (int i = 0; i < Nr; ++i)
        {
            s(0.0);
            f.process(s.control(), s.observation());
            s.stream<' '>(fo);
            streamParticles<' '>(fo, f.particles());
            fo << endl;
        }
    }
    {
        ofstream fo("last.csv");
        streamParticles<'\n'>(fo, f.particles());
    }
    gubg::Plot p_state, p_obs;
    p_state.scatter(s.timeAry, s.stateAry);
    p_state.scatter(s.timeAry, s.observationAry);
    std::this_thread::sleep_for(std::chrono::seconds(3));
    return 0;
}
