#ifndef HEADER_gubg_math_stat_Mixture_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_stat_Mixture_hpp_ALREADY_INCLUDED

#include "gubg/math/stat/Distribution.hpp"
#include "gubg/distribution/Uniform.hpp"
#include <vector>

#define GUBG_MODULE "Mixture"
#include "gubg/log/begin.hpp"
namespace gubg
{
    template <typename Component>
        class Mixture: public Distribution_crtp<Mixture<Component>, typename Component::value_type>
        {
            private:
                typedef Distribution_crtp<Mixture<Component>, typename Component::value_type> Base;

            public:
                typedef std::vector<Component> Components;
                typedef std::vector<double> Weights;
                typedef typename Base::value_type value_type;

                Mixture(const std::size_t nrComponents):
                    components_(nrComponents), weights_(nrComponents)
                {
                    std::fill(weights_.begin(), weights_.end(), 1.0/nrComponents);
                }

                size_t nrComponents() const {return components_.size();}

                Components &components() {return components_;}
                Component &component(size_t ix) {return components_[ix];}
                Weights &weights() {return weights_;}

            private:
                bool invariants_() const
                {
                    if (components_.size() != weights_.size())
                        return false;
                }

                friend class Distribution_crtp<Mixture<Component>, typename Component::value_type>;
                bool distribution_draw(value_type &res)
                {
                    S();
                    auto u = distribution::drawUniform();
                    L(u);
                    size_t ix = 0;
                    for (auto w: weights_)
                    {
                        if (u <= w)
                            break;
                        u -= w;
                        ++ix;
                    }
                    if (ix >= nrComponents())
                        ix = nrComponents()-1;
                    return components_[ix].draw(res);
                }

                Components components_;
                Weights weights_;
        };
}
#include "gubg/log/end.hpp"

#endif
