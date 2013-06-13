#ifndef HEADER_gubg_planning_Planning_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Planning_hpp_ALREADY_INCLUDED

#include "gubg/planning/Codes.hpp"
#include "gubg/planning/Types.hpp"
#include "gubg/planning/Task.hpp"
#include "gubg/tree/dfs/Leafs.hpp"
#include "gubg/OnlyOnce.hpp"
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

                void stream(std::ostream &os) const
                {
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

            private:
                typedef std::map<Worker, Efficiency> EfficiencyPerWorker;
                EfficiencyPerWorker efficiencyPerWorker_;

                struct TaskPart
                {
                    Sweat sweat;
                    Task::Ptr task;

                    TaskPart():sweat(0){}
                };
                typedef std::list<TaskPart> TaskParts;
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

				static Sweat eps_() {return 0.0001;}
        };
    }
}
namespace std
{
    ostream &operator<<(ostream &os, const gubg::planning::Planning &pl)
    {
        pl.stream(os);
        return os;
    }
}
#include "gubg/log/end.hpp"

#endif
