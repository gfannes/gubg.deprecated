#ifndef HEADER_gubg_threading_Queue_hpp_ALREADY_INCLUDED
#define HEADER_gubg_threading_Queue_hpp_ALREADY_INCLUDED

#include "boost/thread.hpp"
#include "boost/thread/mutex.hpp"
#include "boost/thread/condition_variable.hpp"
#include <memory>
#include <deque>

namespace gubg
{
    namespace threading
    {
        template <typename MessageT>
            class Queue
            {
                public:
                    typedef MessageT Message;
                    struct Closed{};
                    Queue():
                        isClosed_(false){}
                    void push(std::unique_ptr<Message> message) throw (Closed)
                    {
                        boost::mutex::scoped_lock lock(mutex_);
                        checkClosed_();
                        messages_.push_back(message.release());
                        somethingChanged_.notify_all();
                    }
                    std::unique_ptr<Message> pop() throw (Closed)
                    {
                        std::unique_ptr<Message> ret;
                        {
                            boost::mutex::scoped_lock lock(mutex_);
                            while (!ret.get())
                            {
                                checkClosed_();
                                if (messages_.empty())
                                    somethingChanged_.wait(lock);
                                else
                                {
                                    ret.reset(messages_.front());
                                    messages_.pop_front();
                                }
                            }
                        }
                        return ret;
                    }
                    bool pop(std::unique_ptr<Message> &res, int timeout_msec) throw (Closed)
                    {
                        //Not implemented yet
                        return false;
                    }
                    void close()
                    {
                        boost::mutex::scoped_lock lock(mutex_);
                        isClosed_ = true;
                        somethingChanged_.notify_all();
                    }
                private:
                    //Assumes that mutex_ is held
                    void checkClosed_() const throw (Closed)
                    {
                        if (isClosed_)
                            throw Closed();
                    }
                    bool isClosed_;
                    std::deque<Message*> messages_;
                    boost::mutex mutex_;
                    boost::condition_variable somethingChanged_;
            };
    }
}

#endif
