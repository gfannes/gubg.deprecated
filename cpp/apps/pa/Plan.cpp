#include "pa/Plan.hpp"
#include "pa/Model.hpp"
#include "gubg/tree/dfs/Iterate.hpp"
#include "gubg/planning/Planning.hpp"
#include <string>
#include <fstream>
#include <map>
#include <set>
using namespace pa;
using namespace std;

#define GUBG_MODULE_ "Plan"
#include "gubg/log/begin.hpp"
namespace 
{
    struct Planner
    {
        gubg::planning::Planning planning;
        gubg::planning::Line *line;
		int lineDepth;
        std::ostringstream parseError_;
		set<string> lineNames_;

        Planner()
        {
            using namespace gubg::planning;
            planning.resources.addWorker("gfa", 0.8);
            planning.resources.addWorker("wba", 0.5);
            for (auto d: workDays(400))
                planning.resources.addDay(d);
            for (auto d: dayRange(Day(2013, 7, 4), Day(2013, 7, 20)))
                planning.resources.absence("gfa", d);
            for (auto d: dayRange(Day(2013, 7, 1), Day(2013, 7, 12)))
                planning.resources.absence("wba", d);

            Workers workers;
            workers.push_back("gfa");
            planning.getLine("XX").setMaxSweatPerDay(1.0).setWorkers(workers);
            workers.push_back("wba");
            planning.getLine("BLITS_L").setMaxSweatPerDay(2.0).setWorkers(workers);
            planning.getLine("BLITS_S").setMaxSweatPerDay(2.0).setWorkers(workers);
        }

		void addLineName(const string &lineName)
		{
			lineNames_.insert(lineName);
		}

        ReturnCode run()
        {
			MSS_BEGIN(ReturnCode);
            if (!parseError_.str().empty())
            {
                std::cout << parseError_.str();
				MSS_L(ParsingError);
            }
			MSS(planning.run());
			MSS_END();
        }

        template <typename Path>
            bool open(Node &n, Path &p)
            {
                SS(n.desc, p.size());
                const auto total = n.value*n.fraction;
                if (total <= 0)
                    return false;
                switch (p.size())
                {
                    case 0:
						line = 0;
						lineDepth = 0;
                        break;
					default:
						if (!line)
						{
							if (!lineNames_.empty() && lineNames_.count(n.desc) == 0)
								return true;
							L("Found new line " << n.desc);
							line = &planning.getLine(n.desc);
							lineDepth = p.size();
						}
						else
						{
							if (p.size() > lineDepth + 1)
								return false;
                            gubg::planning::Task task(n.desc, total);
                            auto deadline = n.attributes.find("deadline");
                            if (deadline != n.attributes.end())
                            {
                                L("Found deadline attribute");
                                auto dl = gubg::planning::Day(deadline->second);
                                if (!dl.isValid())
                                    parseError_ << "Could not parse deadline attribute for node " << n.desc << ": \"" << deadline->second << "\"" << std::endl;
                                else
                                    task.deadline = dl;
                            }
                            line->addTask(task);
						}
						break;
                }
                return true;
            }

        template <typename Path>
            void close(Node &n, Path &p)
			{
				if (line && lineDepth == p.size())
				{
					line = 0;
				}
			}
    };
}
pa::ReturnCode Plan::execute(const Options &options)
{
	MSS_BEGIN(ReturnCode, "Planning");

	Planner planner;
	for (auto lineName: options.lines)
		planner.addLineName(lineName);
	gubg::tree::dfs::iterate(model(), planner);
	MSS(planner.run());
	planner.planning.stream(cout);
	if (!options.output.name().empty())
	{
		ofstream fo(options.output.name());
		planner.planning.stream(fo);
	}

	MSS_END();
}
