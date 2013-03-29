#ifndef HEADER_gubg_math_stat_Mixture_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_stat_Mixture_hpp_ALREADY_INCLUDED

#include <vector>

#define GUBG_MODULE "Mixture"
#include "gubg/log/begin.hpp"
namespace gubg
{
    template <typename Component>
        class Mixture
        {
            public:
                typedef typename Component::value_type value_type;

                Mixture(const std::size_t nrComponents):
                    components_(nrComponents), weights_(nrComponents)
                {
                    std::fill(weights_.begin(), weights_.end(), 1.0/nrComponents);
                }

                bool draw(value_type &res)
                {
                    return false;
                }

            private:
                bool invariants_() const
                {
                    if (components_.size() != weights_.size())
                        return false;
                }
                typedef std::vector<Component> Compontents;
                Component components_;
                typedef std::vector<double> Weights;
                Weights weights_;
        };
}
#include "gubg/log/end.hpp"

#endif
