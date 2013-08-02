#ifndef HEADER_da_Tasks_hpp_ALREADY_INCLUDED
#define HEADER_da_Tasks_hpp_ALREADY_INCLUDED

#include "da/Codes.hpp"
#include <list>
#include <memory>
#include <ostream>

namespace da
{
    struct Options
    {
        bool doFix;
		std::vector<std::string> includePaths;

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

namespace std
{
	inline ostream &operator<<(ostream &os, const da::Options &options)
	{
		os << STREAM(options.doFix) << endl;
		if (!options.includePaths.empty())
		{
			os << "Include paths:" << endl;
			for (auto ip: options.includePaths)
				os << ip << endl;
		}
		return os;
	}
}

#include "da/FixIncludeGuards.hpp"
#include "da/CompileExe.hpp"

#endif
