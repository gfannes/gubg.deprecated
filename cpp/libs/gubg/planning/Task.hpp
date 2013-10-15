#ifndef HEADER_gubg_planning_Task_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Task_hpp_ALREADY_INCLUDED

#include "gubg/planning/Types.hpp"
#include "gubg/planning/Day.hpp"
#include "gubg/tree/dfs/Iterate.hpp"
#include <string>
#include <vector>
#include <list>
#include <map>
#include <ostream>

#define GUBG_MODULE "Task"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace planning
    {
        class Task;
        typedef std::shared_ptr<Day> Deadline;
		struct CompareDeadlines
		{
			bool operator()(Deadline lhs, Deadline rhs)
			{
				return *lhs < *rhs;
			}
		};
		typedef std::multimap<Deadline, std::shared_ptr<Task>, CompareDeadlines> TasksPerDeadline;

        namespace priv
        {
            class Printer
            {
                public:
                    Printer(std::ostream &os):os_(os){}
                    template <typename N, typename P>
                        bool open(N &n, P &p) const
                        {
							if (p.empty())
								os_ << n.fullName() << std::endl;
							else
							{
								os_ << std::string(2*p.size(), ' ') << n.name;
								if (n.deadline)
									os_	<< " deadline: " << *n.deadline;
								if (n.workers && !n.workers->empty())
								{
									os_ << " workers: ";
									for (auto w: *n.workers)
										os_ << " " << w;
								}
								os_ << " effort: " << n.sweat << std::endl;
							}
                            return true;
                        }
                    template <typename N, typename P>
                        void close(N &n, P &p) const
                        {
                        }
                private:
                    std::ostream &os_;
            };
            class DistributeWorkers
            {
                public:
                    template <typename N, typename P>
                        bool open(N &n, P &p) const
                        {
                            if (!n.workers && !p.empty())
                                n.workers = p.back()->workers;
                            return true;
                        }
                    template <typename N, typename P>
                        void close(N &n, P &p) const
                        {
                        }
            };
            class DistributeDeadline
            {
                public:
                    template <typename N, typename P>
                        bool open(N &n, P &p) const
                        {
                            if (!n.deadline && !p.empty())
                                n.deadline = p.back()->deadline;
                            return true;
                        }
                    template <typename N, typename P>
                        void close(N &n, P &p) const
                        {
                        }
            };
            class CheckDeadlines
            {
                public:
                    mutable bool same;
                    mutable Deadline deadline;
                    CheckDeadlines():same(true){}
                    template <typename N, typename P>
                        bool open(N &n, P &p) const
                        {
                            if (!deadline)
                                deadline = n.deadline;
                            else if (deadline != n.deadline)
                            {
                                same = false;
                                return false;
                            }
                            return true;
                        }
                    template <typename N, typename P>
                        void close(N &n, P &p) const
                        {
                        }
            };
            class AggregateSweat
            {
                public:
                    template <typename N, typename P>
                        bool open(N &n, P &p) const
                        {
							n.cumulSweat = n.sweat;
							return true;
                        }
                    template <typename N, typename P>
                        void close(N &n, P &p) const
                        {
							if (p.empty())
								return;
							p.back()->cumulSweat += n.cumulSweat;
                        }
            };
            class AggregateStartStop
            {
                public:
                    template <typename N, typename P>
                        bool open(N &n, P &p) const
                        {
							return true;
                        }
                    template <typename N, typename P>
                        void close(N &n, P &p) const
                        {
							if (p.empty())
								return;
							N &parent = *p.back();
							if (!parent.start.isValid() || n.start < parent.start)
								parent.start = n.start;
							if (!parent.stop.isValid() || parent.stop < n.stop)
								parent.stop = n.stop;
                        }
            };
            class CollectTasksPerDeadline
            {
                public:
                    CollectTasksPerDeadline(TasksPerDeadline &tpd):tpd_(tpd){}
                    template <typename N, typename P>
                        bool open(N &n, P &p) const
                        {
                            CheckDeadlines checkDeadlines;
                            tree::dfs::iterate_ptr(n, checkDeadlines);
                            if (checkDeadlines.same)
                            {
                                tpd_.insert(std::make_pair(n.deadline, n.shared_from_this()));
                                return false;
                            }
                            return true;
                        }
                    template <typename N, typename P>
                        void close(N &n, P &p) const
                        {
                        }
                private:
                    TasksPerDeadline &tpd_;
            };
        }

        class Task: public std::enable_shared_from_this<Task>
        {
            public:
                typedef std::string Name;
                typedef std::string Category;
                typedef std::shared_ptr<Task> Ptr;
                typedef std::weak_ptr<Task> WPtr;
                typedef std::shared_ptr<Task> Child;
                typedef std::vector<Child> Childs;

                Name name;
				Category category;
                Sweat sweat;
                Sweat cumulSweat;
                Day start;
                Day stop;
                Deadline deadline;
                WorkersPtr workers;
                Childs childs;
                WPtr parent;

                static Ptr create(Name n){return Ptr(new Task(n));}

				size_t id() const {return (size_t)this;}

				void setCategory(std::string cat) {category = std::move(cat);}

                Ptr addChild(Name n)
                {
                    assert(invariants_());
                    Ptr child;
                    if (sweat == 0)
                    {
						//We only allow leafs to have sweat
                        childs.push_back(child = Task::create(n));
                        child->parent = shared_from_this();
                        assert(child->invariants_());
                    }
                    assert(invariants_());
                    return child;
                }
                void setDeadline(Day day)
                {
                    assert(invariants_());
                    deadline.reset(new Day(day));
                    assert(invariants_());
                }
                void setWorkers(Workers ws)
                {
                    assert(invariants_());
                    workers.reset(new Workers(ws));
                    assert(invariants_());
                }
                void setSweat(Sweat sw)
                {
                    assert(invariants_());
                    sweat = sw;
                    assert(invariants_());
                }

                void distributeWorkers()
                {
                    tree::dfs::iterate_ptr(*this, priv::DistributeWorkers());
                }
                void distributeDeadlines()
                {
                    tree::dfs::iterate_ptr(*this, priv::DistributeDeadline());
                }
				void aggregateSweat()
				{
                    tree::dfs::iterate_ptr(*this, priv::AggregateSweat());
				}
				void aggregateStartStop()
				{
                    tree::dfs::iterate_ptr(*this, priv::AggregateStartStop());
				}

                TasksPerDeadline tasksPerDeadline()
                {
                    TasksPerDeadline tpd;
                    tree::dfs::iterate_ptr(*this, priv::CollectTasksPerDeadline(tpd));
                    return tpd;
                }

                bool isPlanned() const {return start.isValid() && stop.isValid();}

                void stream(std::ostream &os) const
                {
                    tree::dfs::iterate_ptr(*this, priv::Printer(os));
                }

				Name fullName() const
				{
					std::list<std::string> parts;
					auto n = shared_from_this();
					while (n)
					{
						parts.push_back(n->name);
						n = n->parent.lock();
					}
					std::ostringstream oss;
					for (auto it = parts.rbegin(); it != parts.rend(); ++it)
						oss << *it << "/";
					return oss.str();
				}

            private:
                Task(Name n):name(n), sweat(0), cumulSweat(0)
			{
				SS(name);
			}

                bool invariants_() const
                {
                    if (!childs.empty() && sweat != 0)
                        return false;
                    return true;
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
    inline ostream &operator<<(ostream &os, const gubg::planning::Task &task)
    {
        task.stream(os);
        return os;
    }
}
#include "gubg/log/end.hpp"

#endif
