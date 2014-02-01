#ifndef HEADER_gubg_trace_Scope_hpp_ALREADY_INCLUDED
#define HEADER_gubg_trace_Scope_hpp_ALREADY_INCLUDED

#include "gubg/trace/Msg.hpp"
#include "gubg/trace/Mgr.hpp"

namespace gubg { namespace trace { 
    class Scope
    {
        public:
            Scope(const Msg &msg)
            {
                Mgr::instance().open(msg);
            }
            ~Scope()
            {
                Mgr::instance().close();
            }
        private:
    };
} } 

#endif
