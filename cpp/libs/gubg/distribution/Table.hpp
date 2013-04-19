#ifndef HEADER_gubg_distribution_Table_hpp_ALREADY_INCLUDED
#define HEADER_gubg_distribution_Table_hpp_ALREADY_INCLUDED

#include "gubg/distribution/Engine.hpp"
#include <vector>
#include <random>

#define GUBG_MODULE "Table"
#include "gubg/log/begin.hpp"
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
                    Table(size_t nr)
                    {
                        std::vector<T> probs(nr);
                        std::fill(probs.begin(), probs.end(), 1.0/nr);
                        setProbs(probs);
                    }
                    template <typename UnnormProbs>
                        Table(const UnnormProbs &unnormProbs):
                            cumulProbs_(convertToCumul_(unnormProbs)) { }

                    template <typename UnnormProbs>
                        void setProbs(const UnnormProbs &unnormProbs)
                        {
                            cumulProbs_ = convertToCumul_(unnormProbs);
                        }

                    bool generate(size_t &ix) const
                    {
                        S();
                        const size_t s = cumulProbs_.size();
                        if (s == 0)
                            return false;
                        std::uniform_real_distribution<T> rng(0.0, cumulProbs_[s-1]);
                        const auto rn = rng(gubg::distribution::engine);
                        for (ix = 0; ix < s; ++ix)
                            if (rn <= cumulProbs_[ix])
                                return true;
                        ix = s-1;
                        return true;
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
#include "gubg/log/end.hpp"

#endif
