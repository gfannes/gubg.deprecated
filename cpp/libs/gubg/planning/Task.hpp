#ifndef HEADER_gubg_planning_Task_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Task_hpp_ALREADY_INCLUDED

#include "gubg/planning/Types.hpp"
#include "gubg/planning/Day.hpp"
#include <string>
#include <vector>
#include <ostream>

#define GUBG_MODULE_ "Task"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace planning
    {
        class Task
        {
            public:
                typedef std::string Name;

                Name name;
                Sweat sweat;
                Day start;
                Day stop;
                Day deadline;
                Workers workers;
                Sweat maxSweatPerDay;

                Task(Name n, Sweat s):name(n), sweat(s){}

                bool isPlanned() const {return start.isValid() && stop.isValid();}

                void stream(std::ostream &os) const
                {
                    //os << "Task \"" << name << "\"";
                    os << "Task " << STREAM(name, sweat, start, stop);
                }
        };
        typedef std::vector<Task> Tasks;
        struct TaskRange
        {
            Tasks::iterator begin(){return b;}
            Tasks::iterator end(){return e;}
            Task &front(){return *b;}
            Task &back(){return *(e-1);}
            size_t size() const {return e - b;}

            Tasks::iterator b;
            Tasks::iterator e;
        };
    }
}
namespace std
{
    ostream &operator<<(ostream &os, const gubg::planning::Task &task)
    {
        task.stream(os);
        return os;
    }
}
#include "gubg/log/end.hpp"

#endif
