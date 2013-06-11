#ifndef HEADER_pa_ShowDebug_hpp_ALREADY_INCLUDED
#define HEADER_pa_ShowDebug_hpp_ALREADY_INCLUDED

#include "pa/Tasks.hpp"
#include "pa/Types.hpp"
#include "gubg/file/File.hpp"

namespace pa
{
    class ShowDebug: public ITask
    {
        public:
            static Ptr create(){return Ptr(new ShowDebug());}

            virtual ReturnCode execute(const Options &);

        private:
            ShowDebug(){}
    };
}

#endif
