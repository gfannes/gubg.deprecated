#ifndef HEADER_gubg_trace_Mgr_hpp_ALREADY_INCLUDED
#define HEADER_gubg_trace_Mgr_hpp_ALREADY_INCLUDED

#include "gubg/trace/Msg.hpp"
#include "gubg/Singleton.hpp"
#include <thread>

#define GUBG_MODULE_ "trace::Mgr"
#include "gubg/log/begin.hpp"
namespace gubg { namespace trace { 
    class Mgr
    {
        public:
            static Mgr &instance()
            {
                thread_local Mgr *mgr = 0;
                if (!mgr)
                {
                    S();L("Creating local mgr pointer");
                    mgr = &Singleton<Mgr>::instance();
                }
                return *mgr;
            }
            void open(const Msg &msg)
            {
                S();L("Opening category " << msg.category() << " " << msg.str());
            }
            void close()
            {
                S();L("Closing");
            }

            void operator()()
            {
                while (!quit_)
                {
                }
            }

        private:
            friend class Singleton<Mgr>;
            Mgr(): thread_(std::ref(*this)) {}
            ~Mgr()
            {
                quit_ = true;
                thread_.join();
            }

            volatile bool quit_ = false;
            std::thread thread_;
    };
} } 
#include "gubg/log/end.hpp"

#endif
