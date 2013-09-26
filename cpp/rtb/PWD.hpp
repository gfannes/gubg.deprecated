#ifndef HEADER_rtb_PWD_hpp_ALREADY_INCLUDED
#define HEADER_rtb_PWD_hpp_ALREADY_INCLUDED

#include "gubg/Macro.hpp"
#include "gubg/math/Norm.hpp"
#include "gubg/distribution/Table.hpp"
#include "gubg/distribution/Uniform.hpp"
#include "gubg/distribution/Gaussian.hpp"
#include <array>
#include <algorithm>
#include <complex>
#include <ostream>
#include <cassert>

#define GUBG_MODULE "PWD"
#include "gubg/log/begin.hpp"
namespace rtb
{
    namespace pwd
    {
        typedef std::complex<double> Observation;

        template <size_t Nr, double &Range>
            class State
            {
                public:
                    typedef std::array<double, Nr> Distances;
                    typedef std::array<double, Nr> Angles;

                    State();

                    static double range() {return Range;}

                    const Distances &distances() const {return distances_;}
                    const Angles &angles() const {return angles_;}

                    template <typename Ctr>
                        bool setDistances(const Ctr &ctr);
                    template <typename Ctr>
                        bool setAngles(const Ctr &ctr);

                    double distance(double angle) const;

                    void stream(std::ostream &) const;

                private:
                    bool invariants_() const;

                    void normalizeAngles_();

                    Distances distances_;
                    Angles angles_;
            };

        template <typename S>
            Observation generateGaussianObservation(const S &s, double distance_sigma)
            {
                using namespace gubg::distribution;
                const auto angle = drawUniform(0.0, S::range());
                const auto distance = s.distance(angle);
                S();L(STREAM(angle, distance));
                return std::polar(drawGaussian(distance, distance_sigma), angle);
            }

        template <typename S>
            double observation_prob(const S &s, const Observation &obs, double distance_sigma)
            {
                const auto angle = std::arg(obs);
                const auto distance = s.distance(angle);
                S();L(STREAM(angle, distance));
                return gubg::distribution::gaussian_prob(std::abs(obs), distance, distance_sigma);
            }

        template <typename State_>
            class Model
            {
                public:
                    typedef State_ State;

                    double distance_sigma = 0.01;

                    void updateState(State &state, int) const
                    {
                        {
                            const double sigma = 0.1;
                            auto newDistances = state.distances();
                            gubg::distribution::addGaussianNoise(newDistances, sigma);
                            gubg::math::liftTo(newDistances, 0.05);
                            state.setDistances(newDistances);
                        }

                        {
                            const double sigma = 0.05;
                            auto newAngles = state.angles();
                            gubg::distribution::addGaussianNoise(newAngles, sigma);
                            gubg::math::liftTo(newAngles, 0.05);
                            state.setAngles(newAngles);
                        }
                    }
                    double observation_prob(const Observation &obs, const State &state) const
                    {
                        return pwd::observation_prob(state, obs, distance_sigma);
                    }
                private:
            };

        //State implementation
        //Publics
#define L_TEMPLATE template <size_t Nr, double &Range>
#define L_TYPE State<Nr, Range>
        L_TEMPLATE
            L_TYPE::State()
            {
                distances_.fill(0.0);
                angles_.fill(1.0);
                normalizeAngles_();
                assert(invariants_());
            }
        L_TEMPLATE
            template <typename Ctr>
            bool L_TYPE::setDistances(const Ctr &ctr)
            {
                if (distances_.size() != ctr.size())
                    return false;
                if (!std::all_of(RANGE(ctr), [](double d){return d >= 0.0;}))
                    return false;
                std::copy(RANGE(ctr), distances_.begin());
                assert(invariants_());
                return true;
            }
        L_TEMPLATE
            template <typename Ctr>
            bool L_TYPE::setAngles(const Ctr &ctr)
            {
                if (angles_.size() != ctr.size())
                    return false;
                if (!std::all_of(RANGE(ctr), [](double a){return a >= 0.0;}))
                    return false;
                std::copy(RANGE(ctr), angles_.begin());
                normalizeAngles_();
                assert(invariants_());
                return true;
            }
        L_TEMPLATE
            double L_TYPE::distance(double angle) const
            {
                assert(invariants_());

                auto a = angles_.begin();
                {
                    double cumul = 0;
                    for (; a != angles_.end(); ++a)
                    {
                        cumul += *a;
                        if (angle < cumul)
                            break;
                    }
                    if (a == angles_.end())
                        --a;
                }

                return distances_[a - angles_.begin()];
            }
        L_TEMPLATE
            void L_TYPE::stream(std::ostream &os) const
            {
                os << 'D';
                for (auto d: distances_)
                    os << d << ',';
                os << 'A';
                for (auto a: angles_)
                    os << a << ',';
            }
        //Privates
        L_TEMPLATE
            bool L_TYPE::invariants_() const
            {
                //All angles_ should be positive
                {
                    if (!std::all_of(RANGE(angles_), [](double a){return a >= 0.0;}))
                        return false;
                }
                //The l1 norm of the angles_ should equal Range
                {
                    const auto norm = gubg::math::l1::norm(angles_);
                    if (!gubg::math::isAlmost(Range, norm, 0.000001))
                        return false;
                }
                //All distances_ should be positive
                {
                    if (!std::all_of(RANGE(distances_), [](double d){return d >= 0.0;}))
                        return false;
                }
                return true;
            }
        L_TEMPLATE
            void L_TYPE::normalizeAngles_()
            {
                gubg::math::l1::normalize(angles_, Range);
            }
    }
}
#include "gubg/log/end.hpp"

namespace std
{
    template <size_t Nr, double &Range>
        ostream &operator<<(ostream &os, const rtb::pwd::State<Nr, Range> &state)
        {
            state.stream(os);
            return os;
        }
}

#endif
