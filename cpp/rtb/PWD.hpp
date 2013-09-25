#ifndef HEADER_rtb_PWD_hpp_ALREADY_INCLUDED
#define HEADER_rtb_PWD_hpp_ALREADY_INCLUDED

#include "gubg/Macro.hpp"
#include "gubg/math/Norm.hpp"
#include <array>
#include <algorithm>
#include <complex>
#include <cassert>

namespace rtb
{
    namespace pwd
    {
        typedef std::complex<double> Observation;

        template <size_t Nr, double &Range>
            class State
            {
                public:
                    State();

                    template <typename Ctr>
                        bool setDistances(const Ctr &ctr);
                    template <typename Ctr>
                        bool setAngles(const Ctr &ctr);

                    Observation generateObservation() const;

                private:
                    bool invariants_() const;

                    void normalizeAngles_();

                    typedef std::array<double, Nr> Distances;
                    Distances distances_;
                    typedef std::array<double, Nr> Angles;
                    Angles angles_;
            };

        //Implementation
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
            Observation L_TYPE::generateObservation() const
            {
                Observation ret;
                return ret;
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

#endif
