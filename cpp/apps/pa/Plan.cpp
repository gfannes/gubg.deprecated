#include "pa/Plan.hpp"
#include "pa/Model.hpp"
#include "gubg/tree/dfs/Iterate.hpp"
#include "gubg/planning/Task.hpp"
#include "gubg/planning/Resources.hpp"
//#include "gubg/planning/Planning.hpp"
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
		gubg::planning::Task::Ptr root;
		gubg::planning::Task::Ptr current;
		gubg::planning::Resources resources;
		gubg::planning::TasksPerDeadline tpd_;

#if 0
        gubg::planning::Planning planning;
        gubg::planning::Line *line;
		int lineDepth;
#endif
        std::ostringstream parseError_;
		set<string> lineNames_;

        Planner()
        {
			using namespace gubg::planning;
            resources.addWorker("gfa", 0.8);
            resources.addWorker("wba", 0.5);
            for (auto d: workDays(400))
                resources.addDay(d);
            for (auto d: dayRange(Day(2013, 7, 4), Day(2013, 7, 20)))
                resources.absence("gfa", d);
            for (auto d: dayRange(Day(2013, 7, 1), Day(2013, 7, 12)))
                resources.absence("wba", d);
#if 0
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
#endif
        }

		void addLineName(const string &lineName)
		{
			lineNames_.insert(lineName);
		}

        ReturnCode run()
        {
			MSS_BEGIN(ReturnCode);

			//Use model to build the gubg::planning::Task tree into root
			gubg::tree::dfs::iterate(model(), *this);
            if (!parseError_.str().empty())
            {
                std::cout << parseError_.str();
				MSS_L(ParsingError);
            }

			{
				gubg::planning::Day day;
				MSS(resources.getLastDay(day));
				root->setDeadline(day);
			}
			root->distributeDeadlines();
			root->distributeWorkers();
			root->aggregateSweat();
			tpd_ = root->tasksPerDeadline();
			for (auto p: tpd_)
				resources.planASAP(*p.second);

			MSS_END();
        }
		void stream(std::ostream &os)
		{
			for (auto p: tpd_)
			{
				auto &task = *p.second;
				os << task.start << " -- " << task.stop << ":: " << *p.first << " " << task.fullName()  << "(" << task.cumulSweat << ")" << std::endl;
			}
		}

        template <typename Path>
            bool open(Node &n, Path &p)
            {
                SS(n.desc, p.size(), n.cumul);
                if (n.cumul <= 0)
                    return false;
                switch (p.size())
                {
					case 1:
						if (lineNames_.count(n.desc) == 0)
							return false;
					default:
						if (!root)
						{
							assert(p.empty());
							current = root = gubg::planning::Task::create(n.desc);
						}
						else
							current = current->addChild(n.desc);
						assert(current && root);
						current->setSweat(n.value*n.fraction);
						if (n.attributes.count("allocate"))
						{
							const auto str = n.attributes["allocate"];
							gubg::Strange strange(str); 
							string worker;
							gubg::planning::Workers workers;
							while (strange.popUntil(worker, '|'))
							{
								if (!worker.empty())
									workers.push_back(worker);
							}
							strange.popAll(worker);
							if (!worker.empty())
								workers.push_back(worker);
							current->setWorkers(workers);
						}
						if (n.attributes.count("deadline"))
						{
							gubg::planning::Day deadline(n.attributes["deadline"]);
							if (!deadline.isValid())
								parseError_ << "Could not parse deadline attribute for node " << n.desc << ": \"" << n.attributes["deadline"] << "\"" << std::endl;
							else
								current->setDeadline(deadline);
						}
						break;
#if 0
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
#endif
                }
                return true;
            }

        template <typename Path>
            void close(Node &n, Path &p)
			{
				if (current)
					current = current->parent.lock();
#if 0
				if (line && lineDepth == p.size())
				{
					line = 0;
				}
#endif
			}
    };
}
pa::ReturnCode Plan::execute(const Options &options)
{
	MSS_BEGIN(ReturnCode, "Planning");

	Planner planner;
	for (auto lineName: options.lines)
		planner.addLineName(lineName);
	MSS(planner.run());
	planner.root->stream(cout);
	planner.stream(cout);
#if 0
	planner.planning.stream(cout);
	if (!options.output.name().empty())
	{
		ofstream fo(options.output.name());
		planner.planning.stream(fo);
	}
#endif

	MSS_END();
}
