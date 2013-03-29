#ifndef oeuaoeu
#define oeuaoeu

#include <cstdio>

#define GUBG_MODULE_ "Plot"
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
            void test(double offset)
            {
                std::ostringstream oss;
                oss << "plot sin(x + " << offset << ")";
                send_(oss.str());
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
