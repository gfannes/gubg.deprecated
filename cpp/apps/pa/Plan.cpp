#include "pa/Plan.hpp"
#include "pa/Planner.hpp"
#include <string>
#include <fstream>
#include <map>
#include <set>
using namespace pa;
using namespace std;

#define GUBG_MODULE "Plan"
#include "gubg/log/begin.hpp"
void stream(std::ostream &os, Planner &planner, Plan::Level level, gubg::planning::Format format)
{
	using namespace gubg::planning;
	switch (level)
	{
		case Plan::Overview:
			switch (format)
			{
				case Format::Text:
					for (auto p: planner.root->tasksPerDeadline())
					{
						auto &task = *p.second;
						if (task.stop > *p.first)
							os << "! ";
						else
							os << " ";
						os << task.start << " -- " << task.stop << ":: deadline: " << *p.first << " " << task.fullName() << "(" << task.cumulSweat << ")" << std::endl;
					}
					break;
				case Format::Html:
					os << "<html><body>Not implemented yet</body></html>";
					break;
			}
			break;
		case Plan::Details:
			planner.planning.stream(os, format);
			break;
	}
}
pa::ReturnCode Plan::execute(const Options &options)
{
	MSS_BEGIN(ReturnCode, "Planning");

	using namespace gubg::planning;
	Planner planner;
	MSS(planner.run());
	planner.root->stream(cout);
	stream(cout, planner, level_, Format::Text);
	if (!options.output.name().empty())
	{
		{
			gubg::file::File fn(options.output.name());
			fn.setExtension("txt");
			ofstream fo(fn.name());
			stream(fo, planner, level_, Format::Text);
		}
		{
			gubg::file::File fn(options.output.name());
			fn.setExtension("html");
			ofstream fo(fn.name());
			stream(fo, planner, level_, Format::Html);
		}
	}

	MSS_END();
}
