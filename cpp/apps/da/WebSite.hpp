#ifndef HEADER_da_WebSite_hpp_ALREADY_INCLUDED
#define HEADER_da_WebSite_hpp_ALREADY_INCLUDED

#include "da/Tasks.hpp"

namespace da
{
    class WebSite: public Task_itf
    {
        public:
           static Ptr create() { return Ptr(new WebSite); }

           virtual ReturnCode execute(const Options &);
        private:
    };
}

#endif
