#ifndef HEADER_pa_Planner_hpp_ALREADY_INCLUDED
#define HEADER_pa_Planner_hpp_ALREADY_INCLUDED

#include "pa/Model.hpp"
#include "gubg/tree/dfs/Iterate.hpp"
#include "gubg/planning/Task.hpp"
#include "gubg/planning/Planning.hpp"
#include <sstream>

#define GUBG_MODULE "Planner"
#include "gubg/log/begin.hpp"
namespace pa
{
	struct Planner
	{
		gubg::planning::Task::Ptr root;
		gubg::planning::Task::Ptr current;
		gubg::planning::Planning planning;
		std::ostringstream parseError_;
		const std::string categoryName;

		Planner(const std::string &catName = ""):
			categoryName(catName)
		{
			using namespace gubg::planning;
			planning.addWorker("gfa", 0.7);
			planning.addWorker("wba", 0.4);
			gubg::OnlyOnce upgradeWBA;
			for (auto d: workDays(200))
			{
#if 0
				if (d >= Day(2013,8,1) && upgradeWBA())
					planning.addWorker("wba", 0.5);
#endif
				planning.addDay(d);
			}
#ifdef PAST
			for (auto d: dayRange(Day(2013, 7, 4), Day(2013, 7, 18)))
				planning.absence("gfa", d);
			for (auto d: dayRange(Day(2013, 7, 22), Day(2013, 8, 2)))
				planning.absence("wba", d);
			planning.absence("wba", Day(2013, 8, 16));
#endif
			planning.absence("wba", Day(2013, 11, 28));

			{
				auto verlof = dayRange(Day(2013, 12, 25), Day(2014, 1, 1));
	//			verlof.push_back(Day(2013, 8, 15));
				verlof.push_back(Day(2013, 11, 1));
				verlof.push_back(Day(2013, 11, 11));
				for (auto d: verlof)
				{
					planning.absence("gfa", d);
					planning.absence("wba", d);
				}
			}

			//Quality week
			for (auto d: dayRange(Day(2014, 3, 31), Day(2014, 4, 4)))
			{
				planning.absence("gfa", d);
				planning.absence("wba", d);
			}
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

		template <typename Path>
			bool open(Node &n, Path &p)
			{
				SS(n.desc, p.size(), n.total());
				if (n.total() <= 0)
					return false;
				if (!root)
				{
					assert(p.empty());
					//We do not put the root name, it is typically not a task and causes only bloat in the output
					current = root = gubg::planning::Task::create("");
				}
				else
					current = current->addChild(n.desc);
				assert(root);
				if (!current)
					std::cout << "ERROR::Could not add new child \"" << n.desc << "\", only a leaf node can contain sweat" << std::endl;
				assert(current);
				current->setSweat(n.value*n.fraction);
				if (n.attributes.count("allocate"))
				{
					const auto str = n.attributes["allocate"];
					gubg::Strange strange(str); 
					std::string worker;
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
				if (n.attributes.count(categoryName) > 0)
				{
					L("Found a category");
					current->setCategory(n.attributes[categoryName]);
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
#include "gubg/log/end.hpp"

#endif
