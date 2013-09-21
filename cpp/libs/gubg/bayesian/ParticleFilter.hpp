#ifndef HEADER_gubg_bayesian_ParticleFilter_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bayesian_ParticleFilter_hpp_ALREADY_INCLUDED

#include "gubg/distribution/Table.hpp"
#include <vector>
#include <cstddef>
#include <ostream>
#include <cassert>

#define GUBG_MODULE "ParticleFilter"
#include "gubg/log/begin.hpp"
#include "gubg/log/HR.hpp"
namespace gubg
{
    namespace bayesian
    {
        template <typename System>
            class ParticleFilter
            {
                public:
                    typedef typename System::State State;
                    typedef std::vector<State> Particles;

                    ParticleFilter(const System &system, size_t nr): system_(system), particles_(nr, State()), weights_(nr, 0.0) {}
                    ParticleFilter(const System &system, size_t nr, State initState): system_(system), particles_(nr, initState), weights_(nr, 0.0) {}

                    const Particles &particles() const {return particles_;}

                    template <typename Control, typename Observation>
                        void process(Control control, Observation observation)
                        {
                            S();
                            Particles tmp; tmp.swap(particles_);
                            assert(tmp.size() == weights_.size());
                            Weights::iterator w = weights_.begin();
                            for (auto &p: tmp)
                            {
                                system_.updateState(p, control);
                                *w++ = system_.observation_prob(observation, p);
                            }

                            L(log::hr(weights_));
                            gubg::distribution::convertToCumul(weights_);
                            L(log::hr(weights_));
                            particles_.resize(tmp.size());
                            Weights::const_iterator it = weights_.begin();
                            for (auto &p: particles_)
                            {
                                gubg::distribution::generateFromUnnormCumul(it, weights_);
                                const size_t ix = it-weights_.begin();
                                L(ix);
                                p = tmp[it-weights_.begin()];
                            }
                        }

                private:
                    const System &system_;
                    Particles particles_;
                    typedef std::vector<double> Weights;
                    //We add this as a member to reduce allocation and deallocation overhead. This is only used inside operator()()
                    Weights weights_;
            };
    }
}
#include "gubg/log/end.hpp"

#endif
