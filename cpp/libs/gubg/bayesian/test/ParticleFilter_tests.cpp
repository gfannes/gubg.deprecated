#include "gubg/bayesian/ParticleFilter.hpp"
#include "gubg/distribution/Gaussian.hpp"
#include "gubg/distribution/Table.hpp"
#include "gubg/distribution/Estimation.hpp"
#include "gubg/Plot.hpp"
#include "gubg/macro.hpp"
#include <thread>
#include <complex>
#include <cassert>
#include <iostream>
#include <fstream>
using namespace std;

typedef double Control;
typedef double State;
typedef double Observation;

class System
{
    public:
        typedef int Time;

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
            observation_ = gubg::distribution::drawGaussian(0.0, b_*exp(0.5*state_));

            timeAry.push_back(time_);
            controlAry.push_back(control_);
            stateAry.push_back(state_);
            observationAry.push_back(observation_);
        }
        void updateState(State &state, Control control) const
        {
            state = a_*state + gubg::distribution::drawGaussian(control, 1.0);
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

class Filter
{
    public:
        Filter(const System &system, size_t nr, State initState): system_(system), particles_(nr, initState), weights_(nr, 0.0) {}

        void operator()(Control control, Observation observation)
        {
            Particles tmp; tmp.swap(particles_);
            assert(tmp.size() == weights_.size());
            Weights::iterator w = weights_.begin();
            for (auto &p: tmp)
            {
                system_.updateState(p, control);
                *w++ = system_.observation_prob(observation, p);
            }

            gubg::distribution::convertToCumul(weights_);
            particles_.resize(tmp.size());
            Weights::const_iterator it = weights_.begin();
            for (auto &p: particles_)
            {
                gubg::distribution::generateFromUnnormCumul(it, weights_);
                p = tmp[it-weights_.begin()];
            }
        }
        template <char D>
            void stream(ostream &os) const
            {
                double mean; gubg::distribution::computeMean(mean, particles_);
                os << mean << D;
                for (auto p: particles_)
                    os << p << D;
            }

    private:
        const System &system_;
        typedef vector<State> Particles;
        Particles particles_;
        typedef vector<double> Weights;
        //We add this as a member to reduce allocation and deallocation overhead. This is only used inside operator()()
        Weights weights_;
};

int main()
{
    System s;
    Filter f(s, 1000, 0.0);
    {
        const int Nr = 100;
        ofstream fo("data.csv");
        for (int i = 0; i < Nr; ++i)
        {
            s(0.0);
            f(s.control(), s.observation());
            s.stream<' '>(fo);
            f.stream<' '>(fo);
            fo << endl;
        }
    }
    {
        ofstream fo("last.csv");
        f.stream<'\n'>(fo);
    }
    gubg::Plot p_state, p_obs;
    p_state.scatter(s.timeAry, s.stateAry);
    p_state.scatter(s.timeAry, s.observationAry);
    //    std::this_thread::sleep_for(std::chrono::seconds(30));
    return 0;
}
