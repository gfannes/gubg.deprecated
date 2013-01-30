#ifndef HEADER_gubg_Async_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Async_hpp_ALREADY_INCLUDED

#define GUBG_LOG
#include "gubg/logging/Log.hpp"
#include <chrono>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace gubg
{
    template <typename Receiver, typename Job>
        class Async_crtp
        {
            public:
                template <typename JobT>
                    void addJob(JobT job)
                    {
                        {
                            std::lock_guard<std::mutex> lock(jobsMutex_);
                            jobs_.push(job);
                        }
                        signal_.notify_one();
                    }

                void setWorkerCount(const size_t count)
                {
                    const size_t s = workers_.size();
                    if (count == s)
                        return;
                    if (count > s)
                    {
                        //Create more workers
                        for (size_t i = 0; i < count-s; ++i)
                            workers_.push(std::thread(workerThread_, std::ref(receiver_())));
                    }
                    else
                    {
                        //Remove workers
                    }
                }

                static void workerThread_(Receiver &receiver)
                {
                    LOG_S(thread_, "A new worker is starting");
                    while (true)
                    {
                        std::unique_lock<std::mutex> lock(receiver.jobsMutex_);
                        while (receiver.jobs_.empty())
                            receiver.signal_.wait_for(lock, receiver.timeout_);
                        Job job = receiver.jobs_.front();
                        receiver.jobs_.pop();
                        receiver.async_process(job);
                    }
                }

            private:
                Receiver &receiver_(){return static_cast<Receiver&>(*this);}

                typedef std::queue<std::thread> Workers;
                Workers workers_;

                typedef std::queue<Job> Jobs;
                Jobs jobs_;
                std::mutex jobsMutex_;
                std::chrono::milliseconds timeout_;
                std::condition_variable signal_;
        };
}

#endif
