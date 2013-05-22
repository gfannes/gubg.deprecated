#ifndef HEADER_pa_ShowWBS_hpp_ALREADY_INCLUDED
#define HEADER_pa_ShowWBS_hpp_ALREADY_INCLUDED

#include "pa/Tasks.hpp"
#include "pa/Types.hpp"
#include "gubg/file/File.hpp"

namespace pa
{
    class ShowWBS: public ITask
    {
        public:
            static Ptr create(){return Ptr(new ShowWBS());}

            virtual ReturnCode execute(const Options &);

        private:
            ShowWBS(){}
    };
}

#endif
