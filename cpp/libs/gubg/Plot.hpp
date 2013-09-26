#ifndef HEADER_gubg_Plot_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Plot_hpp_ALREADY_INCLUDED

#include "gubg/math/stat/Histogram.hpp"
#include <cstdio>
#include <tuple>

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
            void wait(const std::string &msg = "Press <enter> to continue ...")
            {
                LLL(msg);
                char buffer[256];
                std::gets(buffer);
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
                    return true;
                }
            template <typename Vs>
                void scatter(const Vs &vs, size_t x, size_t y)
                {
                    send_("plot '-' using 1:2 with points");
                    std::ostringstream oss;
                    for (const auto &v: vs)
                    {
                        oss.str("");
                        oss << v[x] << " " << v[y];
                        send_(oss.str());
                    }
                    send_("e");
                }
            //Collater should return [angle, radius]
            template <typename Data, typename Collater>
                void polar(const Data &data, const Collater &collater)
                {
                    send_("set polar");
                    send_("set grid polar");
                    send_("set trange [-pi:pi]");
                    send_("set rrange [0:10]");
                    send_("plot '-' using 1:2 with points");
                    std::ostringstream oss;
                    for (const auto &d: data)
                    {
                        oss.str("");
                        auto t = collater(d);
                        oss << t[0] << ' ' << t[1];
                        send_(oss.str());
                    }
                    send_("e");
                }
            //Collater should return [angle, radius, label]
            template <typename Data, typename Collater>
                void polar_label(const Data &data, const Collater &collater)
                {
                    send_("set polar");
                    send_("set grid polar");
                    send_("set trange [-pi:pi]");
                    send_("set rrange [0:10]");
                    //send_("plot '-' using 1:2 with points");
                    send_("plot '-' using 1:2:3 with points lc variable");
                    std::ostringstream oss;
                    for (const auto &d: data)
                    {
                        oss.str("");
                        auto t = collater(d);
                        oss << t[0] << ' ' << t[1] << ' ' << t[2];
                        send_(oss.str());
                    }
                    send_("e");
                }
            template <typename Data, typename Collater>
                void vectorField(const Data &data, const Collater &collater)
                {
                    send_("plot [0:2] [0:2] '-' using 1:2:3:4 with vectors");
                    std::ostringstream oss;
                    for (const auto &d: data)
                    {
                        oss.str("");
                        auto t = collater(d);
                        oss << t[0] << ' ' << t[1] << ' ' << t[2] << ' ' << t[3];
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
