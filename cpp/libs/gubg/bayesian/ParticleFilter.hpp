#ifndef HEADER_gubg_bayesian_ParticleFilter_hpp_ALREADY_INCLUDED
#define HEADER_gubg_bayesian_ParticleFilter_hpp_ALREADY_INCLUDED

#include "gubg/math/random/Table.hpp"
#include <vector>
#include <cstddef>
#include <ostream>
#include <cassert>

#define GUBG_MODULE "ParticleFilter"
#include "gubg/log/begin.hpp"
#include "gubg/log/HR.hpp"
namespace gubg { namespace bayesian {

    template <typename Model>
        class ParticleFilter
        {
            public:
                typedef typename Model::State State;
                typedef std::vector<State> Particles;

                ParticleFilter(const Model &model, size_t nr): model_(model), particles_(nr, State()), weights_(nr, 0.0) {}
                ParticleFilter(const Model &model, size_t nr, State initState): model_(model), particles_(nr, initState), weights_(nr, 0.0) {}

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
                            model_.updateState(p, control);
                            *w++ = model_.observation_prob(observation, p);
                        }

                        L(log::hr(weights_));
                        gubg::math::random::convertToCumul(weights_);
                        L(log::hr(weights_));
                        particles_.resize(tmp.size());
                        Weights::const_iterator it = weights_.begin();
                        for (auto &p: particles_)
                        {
                            gubg::math::random::generateFromUnnormCumul(it, weights_);
                            const size_t ix = it-weights_.begin();
                            L(ix);
                            p = tmp[it-weights_.begin()];
                        }
                    }

            private:
                const Model &model_;
                Particles particles_;
                typedef std::vector<double> Weights;
                //We add this as a member to reduce allocation and deallocation overhead. This is only used inside operator()()
                Weights weights_;
        };

} }
#include "gubg/log/end.hpp"

#endif
