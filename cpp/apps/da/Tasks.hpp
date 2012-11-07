#ifndef L_tasks
#define L_tasks

#include "da/Codes.hpp"
#include <list>
#include <memory>

namespace da
{
    class ITask
    {
        public:
            typedef std::shared_ptr<ITask> Ptr;
            virtual ReturnCode execute() = 0;
    };
    typedef std::list<ITask::Ptr> Tasks;
}

#include "da/FixIncludeGuards.hpp"

#endif
