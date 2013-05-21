#ifndef HEADER_pa_ShowTotals_hpp_ALREADY_INCLUDED
#define HEADER_pa_ShowTotals_hpp_ALREADY_INCLUDED

#include "pa/Tasks.hpp"
#include "pa/Types.hpp"
#include "gubg/file/File.hpp"

namespace pa
{
    class ShowTotals: public ITask
    {
        public:
            static Ptr create(){return Ptr(new ShowTotals());}

            virtual ReturnCode execute(const Options &);

        private:
            ShowTotals(){}
    };
}

#endif
