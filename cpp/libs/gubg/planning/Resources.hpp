#ifndef HEADER_gubg_planning_Resources_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Resources_hpp_ALREADY_INCLUDED

#include "gubg/planning/Codes.hpp"
#include "gubg/planning/Types.hpp"
#include "gubg/planning/Day.hpp"
#include "gubg/planning/Task.hpp"
#include <string>
#include <map>
#include <ostream>

#define GUBG_MODULE_ "Resources"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace planning
    {
        class Resources
        {
            public:
                typedef double Efficiency;

                void addWorker(Worker worker, Efficiency efficiency)
                {
                    WorkerInfo wi;
                    wi.efficiency = efficiency;
                    infoPerWorker_[worker] = wi;
                }
                void addDay(Day day)
                {
                    DayInfo di;
                    di.day = day;
                    for (auto wi: infoPerWorker_)
                        di.sweatPerWorker[wi.first] = wi.second.efficiency;
                    infoPerDay_[day] = di;
                }
                void absence(Worker worker, Day day)
                {
                    auto i = infoPerDay_.find(day);
                    if (i == infoPerDay_.end())
                        return;
                    auto j = i->second.sweatPerWorker.find(worker);
                    if (j == i->second.sweatPerWorker.end())
                        return;
                    j->second = 0;
                }
                ReturnCode planASAP(Task &task)
                {
					MSS_BEGIN(ReturnCode, STREAM(task));
                    Sweat sweat = task.sweat;
                    gubg::OnlyOnce setStart;
                    for (auto &di: infoPerDay_)
                    {
                        const Day &day = di.first;
                        for (auto &worker: task.workers)
                        {
                            auto it = di.second.sweatPerWorker.find(worker);
                            if (it != di.second.sweatPerWorker.end())
                            {
                                Sweat available = std::min(it->second, task.maxSweatPerDay);
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
					MSS_END();
                }

                void stream(std::ostream &os) const
                {
                    for (auto &di: infoPerDay_)
                    {
                        os << di.first << std::endl;
                        for (auto &p: di.second.sweatPerWorker)
                            os << "\t" << p.first << ": " << p.second << std::endl;
                    }
                }

            private:
                struct WorkerInfo
                {
                    Efficiency efficiency;
                };
                typedef std::map<Worker, WorkerInfo> InfoPerWorker;
                InfoPerWorker infoPerWorker_;

                typedef std::map<Worker, Sweat> SweatPerWorker;
                struct DayInfo
                {
                    Day day;
                    SweatPerWorker sweatPerWorker;
                };
                typedef std::map<Day, DayInfo> InfoPerDay;
                InfoPerDay infoPerDay_;
        };
    }
}
namespace std
{
    ostream &operator<<(ostream &os, const gubg::planning::Resources &resources)
    {
        resources.stream(os);
        return os;
    }
}
#include "gubg/log/end.hpp"

#endif
