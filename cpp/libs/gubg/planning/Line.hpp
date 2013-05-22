#ifndef HEADER_gubg_planning_Line_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Line_hpp_ALREADY_INCLUDED

#include "gubg/planning/Task.hpp"
#include "gubg/OnlyOnce.hpp"
#include <string>
#include <vector>
#include <ostream>
#include <algorithm>

#define GUBG_MODULE "Line"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace planning
    {
        class Line
        {
            public:
                typedef std::string Name;

                Line():maxSweatPerDay_(1.0){}

                Line &setName(Name name) {name_ = name; return *this;}
                Line &setMaxSweatPerDay(Sweat sweat) {maxSweatPerDay_ = sweat; return *this;}
                Line &setWorkers(const Workers &workers) {workers_ = workers; return *this;}

                Line &addTask(Task task)
                {
                    task.maxSweatPerDay = maxSweatPerDay_;
                    task.workers = workers_;
                    tasks_.push_back(task);
                    return *this;
                }

                bool getUnplannedRange(TaskRange &tr)
                {
                    gubg::OnlyOnce first;
                    for (auto it = tasks_.begin(); it != tasks_.end(); ++it)
                    {
                        Task &task = *it;
                        if (!task.isPlanned())
                        {
                            if (first())
                                tr.b = it;
                            tr.e = it+1;
                            if (task.deadline.isValid())
                                break;
                        }
                    }
                    return !first();
                }

                static size_t max_(size_t m, const Task &task) {return std::max(m, task.name.size());}
                void stream(std::ostream &os) const
                {
                    os << "Line \"" << name_ << "\" has " << tasks_.size() << " tasks:" << std::endl;
                    size_t maxTaskName = std::accumulate(tasks_.begin(), tasks_.end(), 0, max_);
                    for (auto &task: tasks_)
                    {
                        os << "\t" << task.start << " => " << task.stop << ": " << task.name << std::string(maxTaskName-task.name.size(), ' ') << " (" << task.sweat << "d";
                        if (task.deadline.isValid())
                            os << ", deadline " << task.deadline;
                        os << ")" << std::endl;
                    }
                }

            private:
                Tasks tasks_;
                Name name_;
                Sweat maxSweatPerDay_;
                Workers workers_;
        };
    }
}
namespace std
{
    ostream &operator<<(ostream &os, const gubg::planning::Line &line)
    {
        line.stream(os);
        return os;
    }
}
#include "gubg/log/end.hpp"

#endif
