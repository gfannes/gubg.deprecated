#ifndef gubg_pointer_Locked_hpp
#define gubg_pointer_Locked_hpp

#include "boost/thread/recursive_mutex.hpp"
#include "boost/thread/condition.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/scoped_ptr.hpp"

namespace gubg
{
    namespace pointer
    {
        template <typename Data, template <typename Data> class Unlocker>
            class Locked
            {
                public:
                    typedef Unlocker<Data> Unlock;
                    friend class Unlocker<Data>;
                    typedef typename Unlocker<Data>::DataPtr DataPtr;

                    //Default constructor, operator() evaluates to false
                    Locked():
                        lockingData_(new LockingData){}
                    //Constructor from any type that can be converted to DataPtr
                    template <typename DataT>
                    Locked(DataT data):
                        lockingData_(new LockingData),
                        data_(data){}

                    bool operator()() const {return data_;}

                private:
                    typedef typename Unlocker<Data>::LockingData LockingData;
                    typedef typename Unlocker<Data>::LockingDataPtr LockingDataPtr;
                    LockingDataPtr lockingData_;
                    DataPtr data_;
            };

        template <typename Data>
            class ThreadSafeUnlocker
            {
                public:
                    typedef boost::recursive_mutex Mutex;
                    typedef boost::condition Condition;
                    struct LockingData
                    {
                        Mutex mutex_;
                        Condition condition_;
                    };
                    typedef boost::shared_ptr<LockingData> LockingDataPtr;
                    typedef boost::shared_ptr<Data> DataPtr;

                    template <typename Locked>
                    ThreadSafeUnlocker(Locked locked):
                        lockingData_(locked.lockingData_),
                        data_(locked.data_),
                        lock_(new Mutex::scoped_lock(lockingData_->mutex_))
                    {
                    }
                    virtual ~ThreadSafeUnlocker()
                    {
                        lock_.reset();
                        lockingData_->condition_.notify_all();
                    }

                    Data &operator*() const {return *data_;}
                    Data *operator->() const {return data_.operator->();}
                    Data *get() const {return data_.get();}
                    bool operator()() const {return data_;}

                    void wait()
                    {
                        lockingData_->condition_.wait(*lock_);
                    }
                    bool timed_wait(boost::system_time timedOut)
                    {
                        return lockingData_->condition_.timed_wait(*lock_, timedOut);
                    }

                private:
                    LockingDataPtr lockingData_;
                    DataPtr data_;
                    typedef boost::scoped_ptr<Mutex::scoped_lock> LockPtr;
                    LockPtr lock_;
            };
    }
}

#endif
