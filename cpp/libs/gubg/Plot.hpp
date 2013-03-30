#ifndef oeuaoeu
#define oeuaoeu

#include "gubg/math/stat/Histogram.hpp"
#include <cstdio>

#define GUBG_MODULE "Plot"
#include "gubg/log/begin.hpp"
namespace gubg
{
    class Plot
    {
        public:
            Plot():
                f_(0)
            {
                f_ = ::popen(gnuplot_, "w");
            }
            ~Plot()
            {
                if (f_)
                    ::pclose(f_);
            }
            void function(const std::string &f)
            {
                std::ostringstream oss;
                oss << "plot " << f;
                send_(oss.str());
            }
            template <typename Xs, typename Ys>
                bool scatter(const Xs &xs, const Ys &ys)
                {
                    if (xs.size() != ys.size())
                        return false;
                    auto ix = xs.begin();
                    auto iy = ys.begin();
                    send_("plot '-' using 1:2 with points");
                    std::ostringstream oss;
                    for (; ix != xs.end(); ++ix, ++iy)
                    {
                        oss.str("");
                        oss << *ix << " " << *iy;
                        send_(oss.str());
                    }
                    send_("e");
                }
            template <typename Xs>
                bool histogram(const Xs &xs, size_t nrBins)
                {
                    S();
                    Histogram h(nrBins);
                    h.learn(xs);
                    send_("plot '-' using 1:2 with lines");
                    std::ostringstream oss;
                    for (const auto &xy: h.shape())
                    {
                        oss.str("");
                        oss << xy.first << " " << xy.second;
                        send_(oss.str());
                    }
                    send_("e");
                }
        private:
            void send_(const std::string &str)
            {
                if (!f_)
                    return;
                std::fputs(str.c_str(), f_);
                std::fputs("\n", f_);
                std::fflush(f_);
            }

            const char *gnuplot_ = "/usr/bin/gnuplot";
            FILE *f_;
    };
}
#include "gubg/log/end.hpp"

#endif
