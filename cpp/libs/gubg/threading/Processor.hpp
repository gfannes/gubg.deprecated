#ifndef HEADER_gubg_threading_Processor_hpp_ALREADY_INCLUDED
#define HEADER_gubg_threading_Processor_hpp_ALREADY_INCLUDED

#include "gubg/threading/Codes.hpp"
#include <memory>
#include <thread>
#include <condition_variable>
#include <queue>

#define GUBG_MODULE "Processor"
#include "gubg/log/begin.hpp"
namespace gubg
{
    namespace threading
    {
        template <typename Job>
            class Processor
            {
                public:
                    typedef std::shared_ptr<Job> JobPtr;

                    Processor(size_t maxJobs):
                        maxJobs_(maxJobs),
                        nrRunningJobs_(0),
                        state_(Stopped){}

                    void operator<<(JobPtr job)
                    {
                        Lock lock(mutex_);
                        queue_.push(job);
                        event_.notify_one();
                    }

                    ReturnCode start()
                    {
                        MSS_BEGIN(ReturnCode);
                        {
                            Lock lock(mutex_);
                            MSS(state_ == Stopped, ProcessorAlreadyRunning);
                            state_ = Starting;
                            event_.notify_one();
                        }
                        std::thread tmpThread(std::ref(*this));
                        tmpThread.swap(processingThread_);
                        MSS_END();
                    }
                    ReturnCode stop()
                    {
                        MSS_BEGIN(ReturnCode);
                        {
                            Lock lock(mutex_);
                            MSS(state_ == Starting || state_ == Running, ProcessorAlreadyStopped);
                            state_ = Stopping;
                            event_.notify_one();
                        }
                        processingThread_.join();
                        MSS_END();
                    }

                    //Thread that will start the processing threads
                    void operator()()
                    {
                        MSS_BEGIN(void);
                        Lock lock(mutex_);
                        bool ok = true;
                        while (ok)
                        {
                            switch (state_)
                            {
                                case Stopping:
                                    if (nrRunningJobs_ == 0 && queue_.empty())
                                        state_ = Stopped;
                                    break;
                                case Stopped: ok = false; break;

                                case Starting: state_ = Running; break;
                                case Running: break;
                            }
                            if (state_ != Running && state_ != Stopping)
                                continue;
                            if (nrRunningJobs_ >= maxJobs_)
                            {
                                L("Maximum number of jobs reached, waiting for an event...");
                                event_.wait(lock);
                                L("Received an event, I will recheck");
                                continue;
                            }
                            if (queue_.empty())
                            {
                                L("Queue is empty, waiting for an event...");
                                event_.wait(lock);
                                L("Received an event, I will recheck");
                                continue;
                            }
                            L("About to start job " << nrRunningJobs_);
                            ++nrRunningJobs_;
                            new Job_(queue_.front(), *this);
                            queue_.pop();
                        }
                        MSS_END();
                    }

                private:
                    struct Job_
                    {
                        JobPtr job_;
                        Processor &outer_;
                        std::thread thread_;
                        Job_(JobPtr job, Processor &outer):
                            job_(job), outer_(outer), thread_(std::ref(*this)){}
                        void operator()()
                        {
                            MSS_BEGIN(void);
                            job_->execute();
                            Lock lock(outer_.mutex_);
                            --outer_.nrRunningJobs_;
                            outer_.event_.notify_one();
                            MSS_END();
                        }
                    };

                    size_t maxJobs_;
                    volatile size_t nrRunningJobs_;

                    typedef std::queue<JobPtr> Queue;
                    typedef std::mutex Mutex;
                    typedef std::unique_lock<Mutex> Lock;
                    typedef std::condition_variable ConditionVariable;
                    Queue queue_;
                    Mutex mutex_;
                    ConditionVariable event_;

                    enum State {Stopped, Starting, Running, Stopping};
                    volatile State state_;
                    std::thread processingThread_;
            };
    }
}
#include "gubg/log/end.hpp"

#endif
