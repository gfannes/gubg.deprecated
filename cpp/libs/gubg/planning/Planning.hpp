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

#define GUBG_MODULE_ "Planning"
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

				//A bit to quick, the workers can vary per subtask
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
                    while (sweat > 0)
                    {
                        SS(sweat);
                        Worker worker;
                        auto dayPlanning = getFirstAvailableDayPlanning_(worker, workers);
                        MSS(dayPlanning != 0, NotEnoughSweatAvailable);
                        auto taskPart = dayPlanning->addTask(task, sweat);
                        MSS(taskPart);
                        L("Assigned " << taskPart->sweat << " to " << worker << " on " << dayPlanning->day);
                    }
#if 0
                    for (auto &di: infoPerDay_)
                    {
                        const Day &day = di.first;
                        for (auto &worker: *task.workers)
                        {
                            auto it = di.second.sweatPerWorker.find(worker);
                            if (it != di.second.sweatPerWorker.end())
                            {
                                //Sweat available = std::min(it->second, task.maxSweatPerDay);
                                Sweat available = it->second;
                                if (sweat <= 0 || available > 0)
                                {
                                    if (setStart())
                                        task.start = day;
                                    if (available >= sweat)
                                    {
                                        it->second -= sweat;
                                        task.stop = day;
                                        MSS_RETURN_OK();
                                    }
                                    sweat -= available;
                                    it->second -= available;
                                }
                            }
                        }
                    }
                    MSS_L(NotEnoughSweatAvailable);
#endif
                    MSS_END();
                }

                void stream(std::ostream &os) const
                {
                    for (const auto &p: dayPlanningsPerWorker_)
                    {
                        os << p.first << std::endl;
                        for (const auto &p2: p.second)
                        {
                            os << "\t" << p2.first << " " << p2.second.sweat << std::endl;
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
                        return availableSweat() > 0;
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
