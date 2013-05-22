#ifndef HEADER_pa_Plan_hpp_ALREADY_INCLUDED
#define HEADER_pa_Plan_hpp_ALREADY_INCLUDED

#include "pa/Tasks.hpp"
#include "pa/Types.hpp"
#include "gubg/file/File.hpp"

namespace pa
{
    class Plan: public ITask
    {
        public:
            static Ptr create(){return Ptr(new Plan());}

            virtual ReturnCode execute(const Options &);

        private:
            Plan(){}
    };
}

#endif
