#ifndef HEADER_pa_Tasks_hpp_ALREADY_INCLUDED
#define HEADER_pa_Tasks_hpp_ALREADY_INCLUDED

#include "pa/Codes.hpp"
#include "gubg/file/File.hpp"
#include <list>
#include <memory>

namespace pa
{
    struct Options
    {
        gubg::file::File input;
        gubg::file::File output;
        std::string value;
        std::string fraction;
        std::string fraction_default;
		std::vector<std::string> lines;
    };
    class ITask
    {
        public:
            typedef std::shared_ptr<ITask> Ptr;
            virtual ReturnCode execute(const Options &) = 0;
    };
    typedef std::list<ITask::Ptr> Tasks;
}

#include "pa/LoadMindMap.hpp"
#include "pa/ShowWBS.hpp"
#include "pa/ShowDebug.hpp"
#include "pa/Plan.hpp"

#endif
