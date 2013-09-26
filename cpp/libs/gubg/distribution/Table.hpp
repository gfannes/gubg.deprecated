#ifndef HEADER_gubg_distribution_Table_hpp_ALREADY_INCLUDED
#define HEADER_gubg_distribution_Table_hpp_ALREADY_INCLUDED

#include "gubg/distribution/Engine.hpp"
#include "gubg/distribution/Uniform.hpp"
#include <vector>
#include <random>
#include <algorithm>

#define GUBG_MODULE "Table"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace distribution
    {
        template <typename Container, typename T = typename Container::value_type>
            void convertToCumul(Container &ctr)
            {
                T sum = T();
                for (auto &cumul: ctr)
                {
                    sum += cumul;
                    cumul = sum;
                }
            }
        template <typename It, typename Container, typename T = typename Container::value_type>
            bool generateFromUnnormCumul(It &it, const Container &ctr)
            {
                S();
                if (ctr.empty())
                    return false;
                std::uniform_real_distribution<T> rng(0.0, ctr.back());
                const auto rn = rng(gubg::distribution::engine);
                it = std::upper_bound(ctr.begin(), ctr.end(), rn);
                if (it == ctr.end())
                    --it;
                return true;
            }
        template <typename It, typename Container, typename T>
            bool generateFromUnnormProb(It &it, const Container &ctr, T sum)
            {
                S();
                if (ctr.empty())
                    return false;
                auto prob = drawUniform(0.0, sum);
                T cumul = 0;
                for (it = ctr.begin(); it != ctr.end(); ++it)
                {
                    cumul += *it;
                    if (prob < cumul)
                        break;
                }
                if (it == ctr.end())
                    --it;
                return true;
            }

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
