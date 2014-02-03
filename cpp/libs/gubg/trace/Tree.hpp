#ifndef HEADER_gubg_trace_Tree_hpp_ALREADY_INCLUDED
#define HEADER_gubg_trace_Tree_hpp_ALREADY_INCLUDED

#include "gubg/trace/Msg.hpp"
#include "gubg/tree/Node.hpp"
#include <memory>
#include <thread>
#include <mutex>
#include <vector>
#include <algorithm>

#define GUBG_MODULE_ "trace::Mgr"
#include "gubg/log/begin.hpp"
namespace gubg { namespace trace { 

    struct Statistics
    {
        typedef std::vector<double> Proportions;
        Proportions proportions;
    };

    class Tree
    {
        private:
            typedef std::chrono::high_resolution_clock Clock;
            struct Data
            {
                Clock::time_point start;
                Clock::time_point stop;
                std::string str;
                size_t category = 0;
            };
            typedef gubg::tree::Node<Data> Node;

        public:
            typedef std::shared_ptr<Tree> Ptr;

            Tree(std::thread::id tid): tid_(tid) {}

            void open(const Msg &msg)
            {
                if (!root_)
                {
                    root_ = Node::create();
                    back_ = root_;
                }
                else
                    back_ = back_.pushChild(Node::create());

                back_.data().start = Clock::now();
                back_.data().str = msg.str();
                back_.data().category = msg.category();
            }
            //Returns false if the thread died
            bool close()
            {
                back_.data().stop = Clock::now();
                back_ = back_.parent();
                return back_;
            }

            struct Computer
            {
                std::vector<Clock::duration> durations;
                size_t level = 0;
                size_t cat;
                Clock::time_point tp;
                bool open(Node n)
                {
                    assert(n);

                    if (level == 0)
                    {
                        cat = n.data().category;
                        tp = n.data().start;
                    }

                    const auto newCat = n.data().category;
                    if (cat != newCat)
                    {
                        const auto ltp = n.data().start;
                        addDuration_(cat, ltp - tp);
                        cat = newCat;
                        tp = ltp;
                    }

                    ++level;
                    return true;
                }
                void close(Node n)
                {
                    assert(n);
                    --level;

                    const auto ltp = n.data().stop;
                    if (level == 0)
                    {
                        //Last level, add the last duration
                        addDuration_(cat, ltp - tp);
                    }
                    else
                    {
                        //Not the last level, we expect to have a parent
                        auto p = n.parent();
                        assert(p);
                        const auto newCat = p.data().category;
                        if (cat != newCat)
                        {
                            //We take the stop time from the node itself, _not_ the parent
                            addDuration_(cat, ltp - tp);
                            cat = newCat;
                            tp = ltp;
                        }
                    }
                }
                void addDuration_(size_t cat, Clock::duration dur)
                {
                    if (durations.size() <= cat)
                        durations.resize(cat+1);
                    durations[cat] += dur;
                }
            };
            Statistics statistics() const
            {
                Statistics stats;
                if (!root_)
                    return stats;

                Computer computer;
                {
                    LockGuard lg(mutex_);
                    //We set the stop times from back_ to the root, we don't know these values,
                    //so we take the last time_point from the tree
                    //This does not change the state of the tree, these values where not set yet
                    {
                        Clock::time_point tp = back_.data().start;
                        if (auto ch = back_.lastChild())
                            tp = ch.data().stop;
                        for (auto n = back_; n; n = n.parent())
                            n.data().stop = tp;
                    }
                    iterate_dfs(root_, computer);
                }

                const double sum = std::accumulate(computer.durations.begin(), computer.durations.end(), Clock::duration()).count();
                stats.proportions.resize(computer.durations.size());
                auto it = computer.durations.begin();
                for (auto &prop: stats.proportions)
                {
                    prop = it->count()/sum;
                    ++it;
                }

                return stats;
            }

            void print()
            {
                Printer printer;
                iterate_dfs(root_, printer);
            }

        private:
            struct Printer
            {
                size_t level = 0;
                bool open(Node n)
                {
                    std::cout << std::string(2*level, '*') << n.data().str << std::endl;
                    ++level;
                    return true;
                }
                void close(Node n)
                {
                    --level;
                }
            };

            const std::thread::id tid_;

            Node root_;
            Node back_;
            typedef std::mutex Mutex;
            typedef std::lock_guard<Mutex> LockGuard;
            mutable Mutex mutex_;
    };
} } 
#include "gubg/log/end.hpp"

#endif
