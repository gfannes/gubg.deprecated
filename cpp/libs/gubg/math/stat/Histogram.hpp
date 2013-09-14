#ifndef HEADER_gubg_math_stat_Histogram_hpp_ALREADY_INCLUDED
#define HEADER_gubg_math_stat_Histogram_hpp_ALREADY_INCLUDED

#include "gubg/OnlyOnce.hpp"
#include <cassert>
#include <vector>
#include <cmath>

#define GUBG_MODULE "Histogram"
#include "gubg/log/begin.hpp"
namespace gubg
{
    class Histogram
    {
        public:
            Histogram(const size_t nrBins):
                counts_(nrBins), coef_(0){}

            const double min() const {return min_;}
            const double max() const {return max_;}

            typedef std::vector<std::pair<double, double>> Shape;
            Shape shape() const
            {
                S();
                Shape sh((counts_.size()+1)*2);
                auto it = sh.begin();
                *it++ = std::make_pair(min_, 0.0);
                auto x = min_;
                for (auto c: counts_)
                {
                    L(c);
                    const auto h = c*coef_;
                    *it++ = std::make_pair(x, h);
                    x += width_;
                    *it++ = std::make_pair(x, h);
                }
                *it++ = std::make_pair(max_, 0.0);
                assert(it == sh.end());
                return sh;
            }

            template <typename Xs>
                bool learn(const Xs &xs)
                {
                    S();
                    if (xs.empty())
                        return false;

                    //Find the min and max
                    OnlyOnce init;
                    for (auto x: xs)
                    {
                        if (init())
                            min_ = max_ = x;
                        else if (x < min_)
                            min_ = x;
                        else if (x > max_)
                            max_ = x;
                    }

                    //Compute the width
                    width_ = (max_ - min_)/counts_.size();
                    coef_ = 1.0/width_/xs.size();

                    //Reset the counts_ and fill them according to the data
                    std::fill(counts_.begin(), counts_.end(), 0);
                    for (auto x: xs)
                        ++counts_[ix_(x)];
                }

            double height(double x)
            {
                if (x < min_ || x > max_)
                    return 0.0;
                return counts_[ix_(x)]*coef_;
            }

        private:
            size_t ix_(double x) const
            {
                if (x < min_)
                    return 0;
                if (x > max_)
                    return counts_.size()-1;
                size_t bin = std::floor((x-min_)/width_);
                if (bin >= counts_.size())
                    bin = counts_.size()-1;
                return bin;
            }

            double min_;
            double max_;
            double width_;
            double coef_;
            std::vector<size_t> counts_;
    };
}
#include "gubg/log/end.hpp"

#endif
