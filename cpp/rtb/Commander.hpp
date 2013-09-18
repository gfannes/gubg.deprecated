#ifndef HEADER_rtb_Commander_hpp_ALREADY_INCLUDED
#define HEADER_rtb_Commander_hpp_ALREADY_INCLUDED

#include "Messagetypes.h"
#include <thread>

namespace rtb
{
    class Commander_crtp
    {
        public:
            Commander_crtp(): quit_(false), thread_(std::ref(*this)) {}
            ~Commander_crtp()
            {
                quit_ = true;
                thread_.join();
            }

            void operator()()
            {
            }

        private:
            volatile bool quit_;
            std::thread thread_;
    };
}

#endif
