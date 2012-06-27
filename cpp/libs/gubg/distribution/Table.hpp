#ifndef gubg_distribution_Table_hpp
#define gubg_distribution_Table_hpp

#include "gubg/distribution/Uniform.hpp"
#include <vector>
#include <random>

namespace gubg
{
    namespace distribution
    {
        //Class that can generate discrete values according to a given unnormalized distribution
        template <typename T>
            class Table
            {
                public:
                    Table(){}
                    template <typename UnnormProbs>
                        Table(const UnnormProbs &unnormProbs):
                            cumulProbs_(convertToCumul_(unnormProbs)) { }

                    template <typename UnnormProbs>
                        void setProbs(const UnnormProbs &unnormProbs)
                        {
                            cumulProbs_ = convertToCumul_(unnormProbs);
                        }

                    ReturnCode generate(size_t &ix) const
                    {
                        MSS_BEGIN(ReturnCode);
                        const size_t s = cumulProbs_.size();
                        MSS(s > 0);
                        std::uniform_real_distribution<T> rng(0.0, cumulProbs_[s-1]);
                        const auto rn = rng(gubg::distribution::uniform);
                        for (ix = 0; ix < s; ++ix)
                            if (rn <= cumulProbs_[ix])
                            {
                                MSS_END();
                            }
                        ix = s-1;
                        MSS_END();
                    }

                    T sum() const {return cumulProbs_.back();}

                private:
                    typedef std::vector<T> CumulProbs;

                    template <typename UnnormProbs>
                        static CumulProbs convertToCumul_(const UnnormProbs &unnormProbs)
                        {
                            double sum = 0.0;
                            CumulProbs res(unnormProbs);
                            for (auto &cumul: res)
                            {
                                if (cumul < 0)
                                    cumul = 0;
                                sum += cumul;
                                cumul = sum;
                            }
                            return res;
                        }

                    CumulProbs cumulProbs_;
            };
    }
}

#endif
