#ifndef da_IncludeGuards_hpp
#define da_IncludeGuards_hpp

#include "da/Tasks.hpp"

namespace da
{
    class FixIncludeGuards: public ITask
    {
        public:
            static Ptr create(){return Ptr(new FixIncludeGuards);}
            virtual ReturnCode execute(const Options &);
    };
}

#endif
