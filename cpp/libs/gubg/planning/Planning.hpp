#ifndef HEADER_gubg_planning_Planning_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Planning_hpp_ALREADY_INCLUDED

#include "gubg/planning/Codes.hpp"
#include "gubg/planning/Task.hpp"
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
                    for (auto p: efficiencyPerWorker_)
                        dayPlanningsPerWorker_[p.first][day] = DayPlanning(day, p.second);
                }
                void absence(Worker worker, Day day)
                {
                    dayPlanningsPerWorker_[worker][day] = DayPlanning(day, 0);
                }

                void stream(std::ostream &os) const
                {
                }

            private:
                typedef std::map<Worker, Efficiency> EfficiencyPerWorker;
                EfficiencyPerWorker efficiencyPerWorker_;

                typedef std::list<Task::Ptr> Tasks;
                struct DayPlanning
                {
                    Day day;
                    Sweat sweat;
                    Tasks tasks;

                    DayPlanning():sweat(0){}
                    DayPlanning(Day d, Sweat s):day(d), sweat(s){}
                };
                typedef std::map<Day, DayPlanning> DayPlannings;
                typedef std::map<Worker, DayPlannings> DayPlanningsPerWorker;
                DayPlanningsPerWorker dayPlanningsPerWorker_;
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
