#ifndef gubg_threading_Processor_hpp
#define gubg_threading_Processor_hpp

#include "gubg/threading/Codes.hpp"
#include <memory>
#include <thread>
#include <condition_variable>
#include <queue>

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
                        nrJobs_(0),
                        state_(Stopped){}

                    void operator<<(JobPtr job)
                    {
                        Lock lock(mutex_);
                        queue_.push(job);
                        event_.notify_one();
                    }

                    ReturnCode start()
                    {
                        MSS_BEGIN(ReturnCode, start);
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
                        MSS_BEGIN(ReturnCode, stop);
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
                        MSS_BEGIN(void, processingThread);
                        Lock lock(mutex_);
                        bool ok = true;
                        while (ok)
                        {
                            switch (state_)
                            {
                                case Stopping:
                                    if (nrJobs_ == 0)
                                        state_ = Stopped;
                                    break;
                                case Stopped: ok = false; break;

                                case Starting: state_ = Running; break;
                                case Running: break;
                            }
                            if (state_ != Running && state_ != Stopping)
                                continue;
                            if (nrJobs_ > maxJobs_)
                            {
                                LOG_M("Maximum number of jobs reached, waiting for an event...");
                                event_.wait(lock);
                                LOG_M("Received an event, I will recheck");
                                continue;
                            }
                            if (queue_.empty())
                            {
                                LOG_M("Queue is empty, waiting for an event...");
                                event_.wait(lock);
                                LOG_M("Received an event, I will recheck");
                                continue;
                            }
                            LOG_M("About to start job " << nrJobs_);
                            ++nrJobs_;
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
                            MSS_BEGIN(void, jobThread);
                            job_->execute();
                            Lock lock(outer_.mutex_);
                            --outer_.nrJobs_;
                            outer_.event_.notify_one();
                            MSS_END();
                        }
                    };

                    size_t maxJobs_;
                    volatile size_t nrJobs_;

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

#endif
