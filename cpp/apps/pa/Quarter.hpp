#ifndef HEADER_pa_Quarter_hpp_ALREADY_INCLUDED
#define HEADER_pa_Quarter_hpp_ALREADY_INCLUDED

#include "pa/Tasks.hpp"
#include "pa/Types.hpp"

namespace pa
{
    class Quarter: public ITask
    {
        public:
            static Ptr create(){return Ptr(new Quarter());}

            virtual ReturnCode execute(const Options &);

        private:
            Quarter(){}
    };
}

#endif
