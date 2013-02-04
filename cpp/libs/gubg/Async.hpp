#ifndef HEADER_gubg_Async_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Async_hpp_ALREADY_INCLUDED

#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include <queue>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cassert>

namespace gubg
{
    template <typename Receiver, typename Job>
        class Async_crtp
        {
            public:
                Async_crtp(const size_t nrWorkers):
                    quit_(false),
                    workers_(nrWorkers)
            {
                for (auto &worker: workers_)
                {
                    std::thread thr(threadFunction_, std::ref(*this));
                    worker.swap(thr);
                }
            }
                ~Async_crtp()
                {
                    //We assume Receiver has already stopped the workers
                    //If we do it here, Receiver is already half destructed
                    assert(workers_.empty());
                }

                //Stops all threads, but waits for all pending jobs to finish first
                void join()
                {
                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        quit_ = true;
                    }
                    signal_.notify_all();
                    for (auto &worker: workers_)
                    {
                        if (worker.joinable())
                        {
                            try { worker.join(); }
                            catch (...) { }
                        }
                    }
                    workers_.clear();
                }

                void clearJobs()
                {
                    {
                        std::unique_lock<std::mutex> lock(mutex_);
                        Jobs jobs;
                        jobs_.swap(jobs);
                    }
                    signal_.notify_all();
                }

                template <typename JobT>
                    void addJob(JobT job)
                    {
                        {
                            std::lock_guard<std::mutex> lock(mutex_);
                            jobs_.push(job);
                        }
                        signal_.notify_one();
                    }

            private:
                typedef Async_crtp<Receiver, Job> Outer;
                static void threadFunction_(Outer &outer)
                {
                    outer.threadLoop_();
                }
                void threadLoop_()
                {
                    LOG_S(threadLoop_);
                    while (true)
                    {
                        Job job;
                        {
                            std::unique_lock<std::mutex> lock(mutex_);
                            while (jobs_.empty())
                            {
                                if (quit_)
                                    //We only quit if all jobs are done
                                    return;
                                signal_.wait(lock);
                            }
                            job = jobs_.front();
                            jobs_.pop();
                        }
                        receiver_().async_process(job);
                    }
                }
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}

                bool quit_;
                typedef std::vector<std::thread> Workers;
                Workers workers_;

                typedef std::queue<Job> Jobs;
                Jobs jobs_;
                std::mutex mutex_;
                std::condition_variable signal_;
        };
}

#endif
