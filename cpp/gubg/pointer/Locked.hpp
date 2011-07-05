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
        //Generic smart pointer that _only_ provides access to its pointer via an instance of Locked::Unlock
        //Depending on the specific Unlocker used, this can implement different unlocking strategies
        template <typename DataT, template <typename DataT, typename ...Extra> class Unlocker, typename ...Extra>
            class Locked
            {
                public:
                    typedef DataT Data;
                    typedef Unlocker<Data, Extra...> Unlock;
                    friend class Unlocker<Data, Extra...>;
                    typedef typename Unlocker<Data, Extra...>::DataPtr DataPtr;

                    //Default constructor, operator() evaluates to false
                    Locked():
                        lockingData_(new LockingData){}
                    //Constructor from any type that can be converted to DataPtr
                    template <typename T>
                    Locked(T data):
                        lockingData_(new LockingData),
                        data_(data){}

                    bool operator()() const {return data_;}
                    operator bool () const {return data_;}

                private:
                    typedef typename Unlocker<Data, Extra...>::LockingData LockingData;
                    typedef typename Unlocker<Data, Extra...>::LockingDataPtr LockingDataPtr;
                    LockingDataPtr lockingData_;
                    DataPtr data_;
            };

        //An unlock strategy that makes each instance of Data thread safe. Multiple instances can be access concurrently
        // => Keeps one mutex per instance
        template <typename Data>
            class ThreadSafeInstance
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
                    ThreadSafeInstance(Locked locked):
                        lockingData_(locked.lockingData_),
                        data_(locked.data_),
                        lock_(new Mutex::scoped_lock(lockingData_->mutex_))
                    {
                    }
                    virtual ~ThreadSafeInstance()
                    {
                        lock_.reset();
                        lockingData_->condition_.notify_all();
                    }

                    Data &operator*() const {return *data_;}
                    Data *operator->() const {return data_.operator->();}
                    Data *get() const {return data_.get();}
                    bool operator()() const {return data_;}
                    DataPtr ptr() const {return data_;}

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

        //An unlock strategy that makes each type Data thread safe. Multiple instances of Data _cannot_ be accessed concurrently
        // => Keeps one mutex per Base type
        template <typename Data>
            class ThreadSafeType
            {
                public:
                    typedef boost::recursive_mutex Mutex;
                    typedef boost::condition Condition;
                    struct LockingData
                    {
                        static Mutex mutex_;
                        static Condition condition_;
                    };
                    typedef boost::shared_ptr<LockingData> LockingDataPtr;
                    typedef boost::shared_ptr<Data> DataPtr;

                    template <typename Locked>
                    ThreadSafeType(Locked locked):
                        data_(locked.data_),
                        lock_(new Mutex::scoped_lock(LockingData::mutex_))
                    {
                    }
                    virtual ~ThreadSafeType()
                    {
                        lock_.reset();
                        LockingData::condition_.notify_all();
                    }

                    Data &operator*() const {return *data_;}
                    Data *operator->() const {return data_.operator->();}
                    Data *get() const {return data_.get();}
                    bool operator()() const {return data_;}
                    DataPtr ptr() const {return data_;}

                    void wait()
                    {
                        LockingData::condition_.wait(*lock_);
                    }
                    bool timed_wait(boost::system_time timedOut)
                    {
                        return LockingData::condition_.timed_wait(*lock_, timedOut);
                    }

                private:
                    DataPtr data_;
                    typedef boost::scoped_ptr<Mutex::scoped_lock> LockPtr;
                    LockPtr lock_;
            };
        template <typename Data>
            boost::recursive_mutex ThreadSafeType<Data>::LockingData::mutex_;
        template <typename Data>
            boost::condition ThreadSafeType<Data>::LockingData::condition_;

        //An unlock strategy that makes each type Base and its Derived types thread safe. Multiple instances of Base or Derived _cannot_ be accessed concurrently
        // => Keeps one mutex per Base type
        namespace base
        {
            typedef boost::recursive_mutex Mutex;
            typedef boost::condition Condition;
            template <typename Base>
                struct LockingData
                {
                    static Mutex mutex_;
                    static Condition condition_;
                };
            template <typename Base>
                boost::recursive_mutex LockingData<Base>::mutex_;
            template <typename Base>
                boost::condition LockingData<Base>::condition_;
        }
        template <typename Data, typename Base>
            class ThreadSafeBaseType
            {
                public:
                    typedef base::LockingData<Base> LockingData;
                    typedef boost::shared_ptr<LockingData> LockingDataPtr;
                    typedef boost::shared_ptr<Data> DataPtr;

                    template <typename Locked>
                    ThreadSafeBaseType(Locked locked):
                        data_(locked.data_),
                        lock_(new base::Mutex::scoped_lock(LockingData::mutex_))
                    {
                    }
                    virtual ~ThreadSafeBaseType()
                    {
                        lock_.reset();
                        LockingData::condition_.notify_all();
                    }

                    Data &operator*() const {return *data_;}
                    Data *operator->() const {return data_.operator->();}
                    Data *get() const {return data_.get();}
                    bool operator()() const {return data_;}
                    DataPtr ptr() const {return data_;}

                    void wait()
                    {
                        LockingData::condition_.wait(*lock_);
                    }
                    bool timed_wait(boost::system_time timedOut)
                    {
                        return LockingData::condition_.timed_wait(*lock_, timedOut);
                    }

                private:
                    DataPtr data_;
                    typedef boost::scoped_ptr<base::Mutex::scoped_lock> LockPtr;
                    LockPtr lock_;
            };
    }
}

#endif
