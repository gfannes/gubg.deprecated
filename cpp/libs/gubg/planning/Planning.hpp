#ifndef HEADER_gubg_planning_Planning_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Planning_hpp_ALREADY_INCLUDED

#include "gubg/planning/Codes.hpp"
#include "gubg/planning/Types.hpp"
#include "gubg/planning/Task.hpp"
#include "gubg/tree/dfs/Leafs.hpp"
#include "gubg/OnlyOnce.hpp"
#include "gubg/xml/Builder.hpp"
#include <vector>
#include <map>
#include <set>

#define GUBG_MODULE "Planning"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace planning
    {
        class Planning
        {
            public:
                struct TaskPart
                {
                    Sweat sweat;
                    Task::Ptr task;

                    TaskPart():sweat(0){}
                };
                typedef std::list<TaskPart> TaskParts;

                void addWorker(Worker worker, Efficiency efficiency)
                {
                    efficiencyPerWorker_[worker] = efficiency;
                }
                void addDay(Day day)
                {
                    for (auto &p: efficiencyPerWorker_)
                        dayPlanningsPerWorker_[p.first][day] = DayPlanning(day, p.second);
                }
                void absence(Worker worker, Day day)
                {
                    dayPlanningsPerWorker_[worker][day] = DayPlanning(day, 0);
                }

				bool getLastDay(Day &day) const
				{
                    Day d;
                    for (auto &p: dayPlanningsPerWorker_)
                    {
                        const Day ld = p.second.rbegin()->first;
                        if (!d.isValid() || d < ld)
                            d = ld;
                    }
                    if (!d.isValid())
                        return false;
                    day = d;
					return true;
				}

                ReturnCode plan(Task &taskTree)
                {
                    MSS_BEGIN(ReturnCode, STREAM(taskTree));

                    taskTree.distributeDeadlines();
                    taskTree.distributeWorkers();
                    taskTree.aggregateSweat();

                    auto tpd = taskTree.tasksPerDeadline();
                    for (auto &p: tpd)
                        MSS(planTreeASAP(*p.second));

                    taskTree.aggregateStartStop();

                    MSS_END();
                }

                ReturnCode planTreeASAP(Task &task)
                {
                    MSS_BEGIN(ReturnCode, STREAM(task.fullName()));
                    auto leafTasks = tree::dfs::leafs(task);
                    L("I found " << leafTasks.size() << " leaf tasks");
                    for (auto leaf: leafTasks)
                        MSS(planLeafASAP(*leaf));
                    MSS_END();
                }

                ReturnCode planLeafASAP(Task &task)
                {
                    MSS_BEGIN(ReturnCode, STREAM(task.fullName()));

                    MSS(task.childs.empty());

                    MSS((bool)task.workers);
                    const auto workers = *task.workers;

                    Sweat sweat = task.cumulSweat;
                    gubg::OnlyOnce setStart;
                    while (sweat > eps_())
                    {
                        SS(sweat);
                        Worker worker;
                        auto dayPlanning = getFirstAvailableDayPlanning_(worker, workers);
                        MSS(dayPlanning != 0, NotEnoughSweatAvailable);
                        auto taskPart = dayPlanning->addTask(task, sweat);
                        MSS(taskPart);
                        if (!task.start.isValid())
                            task.start = dayPlanning->day;
                        task.stop = dayPlanning->day;
                        L("Assigned " << taskPart->sweat << " to " << worker << " on " << dayPlanning->day);
                    }

                    MSS_END();
                }

				typedef std::map<Day, TaskParts> TaskPartsPerDay;
				TaskPartsPerDay taskPartsPerDay() const
				{
					S();
					TaskPartsPerDay res;
                    for (auto &dpsw: dayPlanningsPerWorker_)
					{
						for (auto &dp: dpsw.second)
						{
							SS(dp.first);
							for (auto &tp: dp.second.taskParts)
							{
								L(STREAM(*tp.task));
								res[dp.first].push_back(tp);
								L(res.size());
							}
						}
					}
					return res;
				}

                void stream(std::ostream &os, Format format = Format::Text) const
                {
					switch (format)
					{
						case Format::Text:
							streamText_(os);
							break;
						case Format::Html:
							streamHtml_(os);
							break;
					}
				}
				void overview(std::ostream &os, const std::set<Day> &days)
				{
					if (days.empty())
					{
						os << "Empty period specified" << std::endl;
						return;
					}
					std::map<std::string, double> sweatPerCat;
					for (auto it: taskPartsPerDay())
					{
						auto day = it.first;
						if (days.count(day) > 0)
						{
							for (auto &tp: it.second)
							{
								auto task = tp.task;
								auto sweat = tp.sweat;
								if (task)
									sweatPerCat[task->category] += sweat;
							}
						}
					}
					auto b = days.begin();
					auto e = days.end();
					--e;
					for (auto &c_s: sweatPerCat)
						os << c_s.first << "\t" << c_s.second << std::endl;
				}

            private:
                typedef std::map<Worker, Efficiency> EfficiencyPerWorker;
                EfficiencyPerWorker efficiencyPerWorker_;

                struct DayPlanning
                {
                    Day day;
                    Sweat sweat;
                    TaskParts taskParts;

                    DayPlanning():sweat(0){}
                    DayPlanning(Day d, Sweat s):day(d), sweat(s){}

                    Sweat availableSweat() const
                    {
                        Sweat sw = sweat;
                        for (const auto &tp: taskParts)
                            sw -= tp.sweat;
                        return sw;
                    }
                    bool hasSweatAvailable() const
                    {
                        return availableSweat() > eps_();
                    }
                    TaskPart *addTask(Task &task, Sweat &sweatLeft)
                    {
                        TaskPart tp;
                        tp.sweat = std::min(availableSweat(), sweatLeft);
                        if (tp.sweat <= 0)
                            return 0;
                        tp.task = task.shared_from_this();
                        taskParts.push_back(tp);
                        sweatLeft -= tp.sweat;
                        return &taskParts.back();
                    }
                };
                typedef std::map<Day, DayPlanning> DayPlannings;
                typedef std::map<Worker, DayPlannings> DayPlanningsPerWorker;
                DayPlanningsPerWorker dayPlanningsPerWorker_;

                DayPlanning *getFirstAvailableDayPlanning_(Worker &w, const Workers &workers)
                {
                    DayPlanning *res = 0;
                    for (const auto &worker: workers)
                    {
                        auto it = dayPlanningsPerWorker_.find(worker);
                        if (it == dayPlanningsPerWorker_.end())
                            continue;
                        auto &dayPlannings = it->second;
                        for (auto &p: dayPlannings)
                        {
                            if (p.second.hasSweatAvailable())
                            {
                                if (!res)
                                {
                                    //We have nothing yet, we take this one as a starting point
                                    res = &p.second;
                                    w = worker;
                                }
                                else
                                {
                                    //We already have a potential DayPlanning, check if this is better
                                    if (p.second.day < res->day)
                                    {
                                        res = &p.second;
                                        w = worker;
                                    }
                                }
                                //No need to go further for this worker
                                break;
                            }
                        }
                    }
                    return res;
                }
                void streamText_(std::ostream &os) const
				{
					os << "Planning on " << today() << std::endl;
                    for (const auto &p: dayPlanningsPerWorker_)
                    {
                        os << p.first << std::endl;
                        for (const auto &p2: p.second)
                        {
                            if (p2.second.taskParts.empty())
                                //We do not break here, maybe some things are planned in the future
                                continue;
                            os << "\t" << p2.first;
							gubg::OnlyOnce putBehindDate;
                            for (const auto &tp: p2.second.taskParts)
							{
								if (!putBehindDate())
									os << "\t          ";
								os << " (" << tp.sweat << "d) " << tp.task->fullName() << " (work: " << tp.task->sweat << "d)" << std::endl;
							}
                        }
                    }
                }

				struct CompareStart
				{
					//We first compare the start days. If these are the same, we compare the stop days.
					//If we still have a tie, just use the pointer value itself to make sure different tasks with the same start and stop won't get lost
					bool operator()(const Task::Ptr &lhs, const Task::Ptr &rhs) const {return std::make_tuple(lhs->start, lhs->stop, lhs) < std::make_tuple(rhs->start, rhs->stop, rhs);}
				};
                void streamHtml_(std::ostream &os) const
				{
					using namespace gubg::xml::builder;
					Tag html(os, "html", NoShortClose);
					auto body = html.tag("body");
					{
						auto h1 = body.tag("h1");
						h1 << "Planning on " << today();
					}
					auto table = body.tag("table");
					table.attr("border", 0).attr("cellpadding", 0).attr("cellspacing", 0);

					Day globalFirst, globalLast;
                    for (const auto &p: dayPlanningsPerWorker_)
					{
						const auto &dayPlannings = p.second;
                        for (const auto &p2: dayPlannings)
						{
							if (!globalFirst.isValid() || ((p2.second.sweat - p2.second.availableSweat()) > eps_() && p2.second.day < globalFirst))
								globalFirst = p2.second.day;
							if (!globalLast.isValid() || ((p2.second.sweat - p2.second.availableSweat()) > eps_() && p2.second.day > globalLast))
								globalLast = p2.second.day;
						}
					}

                    for (const auto &p: dayPlanningsPerWorker_)
                    {
						const auto worker = p.first;
						const auto &dayPlannings = p.second;

						table.tag("tr").tag("th") << worker;

						Day start = globalFirst;
						Day stop = globalLast;

						//The header with the days
						{
							auto tr = table.tag("tr");
							tr.tag("th") << "date";
							Day firstOfMonth, prev;
							int nr;
							bool toggle = true;
							for (auto d = start; d <= stop; ++d)
							{
								if (!firstOfMonth.isValid())
								{
									firstOfMonth = start;
									nr = 1;
								}
								else
								{
									if (d.day() == 1)
									{
										tr.tag("th").attr("bgcolor", (toggle ? "yellow" : "orange")).attr("colspan", nr) << firstOfMonth << " " << prev;
										firstOfMonth = d;
										nr = 0;
										toggle = !toggle;
									}
									++nr;
								}
								prev = d;
							}
						}

						//Determine the set of all tasks this workers has on its schedule, and sort them based on the start date
						typedef std::set<Task::Ptr, CompareStart> Tasks;
						Tasks tasks;
                        for (const auto &p2: dayPlannings)
							for (const auto &tp: p2.second.taskParts)
								tasks.insert(tp.task);

						//Add the load for this day
						{
							auto tr = table.tag("tr");
							tr.tag("td") << "load";
							for (auto d = start; d <= stop; ++d)
							{
								auto it = dayPlannings.find(d);
								auto td = tr.tag("td");
								if (it == dayPlannings.end())
									td.attr("bgcolor", "grey");
								else
								{
									const auto sweat = it->second.sweat - it->second.availableSweat();
									if (sweat <= 0.2)
											td.attr("bgcolor", "green");
									else if (sweat <= 0.5)
											td.attr("bgcolor", "orange");
									else
											td.attr("bgcolor", "red");
								}
								td << "&nbsp;";
							}
						}

						for (auto task: tasks)
						{
							auto tr = table.tag("tr");
							tr.tag("td").attr("nowrap", "") << task->fullName();
							for (auto d = start; d <= stop; ++d)
							{
								auto it = dayPlannings.find(d);
								auto td = tr.tag("td");
								if (it == dayPlannings.end())
									td.attr("bgcolor", "grey");
								else
								{
									auto &dayPlanning = it->second;
									auto tp = std::find_if(dayPlanning.taskParts.begin(), dayPlanning.taskParts.end(), [&task](const TaskPart &tp){return tp.task == task;});
									if (tp == dayPlanning.taskParts.end())
									{
										if (dayPlanning.sweat > eps_())
											td.attr("bgcolor", "green");
										else
											td.attr("bgcolor", "pink");
									}
									else
										td.attr("bgcolor", "red");
								}
								td << "&nbsp;";
							}
						}
                    }
                }

				static Sweat eps_() {return 0.0001;}
        };
    }
}
namespace std
{
    inline ostream &operator<<(ostream &os, const gubg::planning::Planning &pl)
    {
        pl.stream(os);
        return os;
    }
}
#include "gubg/log/end.hpp"

#endif
