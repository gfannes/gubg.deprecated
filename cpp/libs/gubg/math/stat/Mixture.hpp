#ifndef HEADER_gubg_math_distribution_Mixture_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_distribution_Mixture_hpp_ALREADY_INCLUDED

#include <vector>

namespace gubg
{
    template <typename Mode>
        class Mixture
        {
            public:
                typedef typename Mode::DomainType DomainType;

                Mixture(std::size_t nrModes):
                    modes_(nrModes){}
                bool draw(DomainType &res)
                {
                    return false;
                }
            private:
                std::vector<Mode> modes_;
        };
}

#endif
