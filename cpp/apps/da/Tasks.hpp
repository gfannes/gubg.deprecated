#ifndef L_tasks
#define L_tasks

#include "da/Codes.hpp"
#include <list>
#include <memory>

namespace da
{
    struct Options
    {
        bool doFix;

        Options():
            doFix(false){}
    };
    class ITask
    {
        public:
            typedef std::shared_ptr<ITask> Ptr;
            virtual ReturnCode execute(const Options &) = 0;
    };
    typedef std::list<ITask::Ptr> Tasks;
}

#include "da/FixIncludeGuards.hpp"

#endif
