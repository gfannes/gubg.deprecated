#ifndef HEADER_da_Tasks_hpp_ALREADY_INCLUDED
#define HEADER_da_Tasks_hpp_ALREADY_INCLUDED

#include "da/Codes.hpp"
#include <list>
#include <memory>
#include <ostream>
#include <vector>

namespace da
{
    struct Options
    {
        bool doFix;
        bool doRun;
		std::vector<std::string> trees;
		std::vector<std::string> includes;

        Options():
            doFix(false), doRun(true){}
    };
    class Task_itf
    {
        public:
            typedef std::shared_ptr<Task_itf> Ptr;
            virtual ReturnCode execute(const Options &) = 0;
    };
    typedef std::list<Task_itf::Ptr> Tasks;
}

namespace std
{
	inline ostream &operator<<(ostream &os, const da::Options &options)
	{
		os << STREAM(options.doFix) << endl;
		if (!options.trees.empty())
		{
			os << "Trees:" << endl;
			for (auto tree: options.trees)
				os << tree << endl;
		}
		if (!options.includes.empty())
		{
			os << "Includes:" << endl;
			for (auto inc: options.includes)
				os << inc << endl;
		}
		return os;
	}
}

#include "da/FixIncludeGuards.hpp"
#include "da/FixNewlines.hpp"
#include "da/CompileExe.hpp"
#include "da/UnitTests.hpp"
#include "da/WebSite.hpp"

#endif
