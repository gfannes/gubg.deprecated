#ifndef HEADER_gubg_planning_Types_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Types_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>

namespace gubg
{
    namespace planning
    {
        typedef std::string Worker;
        typedef std::vector<Worker> Workers;
        typedef std::shared_ptr<Workers> WorkersPtr;
        typedef double Sweat;
    }
}

#endif
