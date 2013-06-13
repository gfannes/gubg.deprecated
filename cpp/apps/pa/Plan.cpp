#include "pa/Plan.hpp"
#include "pa/Model.hpp"
#include "gubg/tree/dfs/Iterate.hpp"
#include "gubg/planning/Task.hpp"
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
		gubg::planning::Task::Ptr root;
		gubg::planning::Task::Ptr current;
		gubg::planning::Planning planning;
        std::ostringstream parseError_;
		set<string> lineNames_;

        Planner()
        {
			using namespace gubg::planning;
            planning.addWorker("gfa", 0.8);
            planning.addWorker("wba", 0.5);
			gubg::OnlyOnce upgradeWBA;
            for (auto d: workDays(400))
			{
				if (d >= Day(2013,8,1) && upgradeWBA())
					planning.addWorker("wba", 0.8);
                planning.addDay(d);
			}
            for (auto d: dayRange(Day(2013, 7, 4), Day(2013, 7, 20)))
                planning.absence("gfa", d);
            for (auto d: dayRange(Day(2013, 7, 15), Day(2013, 7, 26)))
                planning.absence("wba", d);
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
				MSS(planning.getLastDay(day));
				root->setDeadline(day);
			}
            planning.plan(*root);

			MSS_END();
        }
		void stream(std::ostream &os)
		{
            planning.stream(os);
#if 0
			for (auto p: tpd_)
			{
				auto &task = *p.second;
				os << task.start << " -- " << task.stop << ":: " << *p.first << " " << task.fullName()  << "(" << task.cumulSweat << ")" << std::endl;
			}
#endif
		}

        template <typename Path>
            bool open(Node &n, Path &p)
            {
                SS(n.desc, p.size(), n.total());
                if (n.total() <= 0)
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
							//We do not put the root name, it is typically not a task and causes only bloat in the output
							current = root = gubg::planning::Task::create("");
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
                }
                return true;
            }

        template <typename Path>
            void close(Node &n, Path &p)
			{
				if (current)
					current = current->parent.lock();
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
	if (!options.output.name().empty())
	{
		ofstream fo(options.output.name());
		planner.stream(fo);
	}

	MSS_END();
}
