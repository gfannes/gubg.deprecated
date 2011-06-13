#include "pointer/Locked.hpp"
#include "testing/Testing.hpp"
#include "threading/InstanceCounter.hpp"
#include "sleep/sleep.hpp"
#include "boost/thread.hpp"
#define GUBG_LOG
#include "logging/Log.hpp"
using namespace gubg::pointer;

namespace
{
    class Data
    {
        public:
            typedef boost::shared_ptr<Data> Ptr;

            Data()
            {
                LOG_SM(Data, "constructor");
                value_ = 0;
            }
            ~Data()
            {
                LOG_SM(Data, "destructor, value_: " << value_);
            }

            static int value_;
    };
    int Data::value_ = 0;
    typedef Locked<Data, ThreadSafeUnlocker> DataPtr;
    typedef Locked<Data, SingletonThreadSafeUnlocker> SDataPtr;

    template <typename DataPtr>
    struct IncrementThread: gubg::InstanceCounter<IncrementThread<DataPtr>>
    {
        IncrementThread(DataPtr data):
            data_(data),
            thread_(boost::ref(*this)){}
        void operator()()
        {
            typename DataPtr::Unlock unlock(data_);
            int v = unlock->value_;
            gubg::nanosleep(0, 100000000);
            unlock->value_ = v+1;
            delete this;
        }
        DataPtr data_;
        boost::thread thread_;
    };
}

int main()
{
    TEST_TAG(main);
    LOG_S(main);
    {
        LOG_SM(constructor, "Constructor");
        {
            LOG_SM(RawPointer, "RawPointer");
            DataPtr data(new Data);
            TEST_TRUE(data());
        }
        {
            LOG_SM(SharedPointer, "SharedPointer");
            Data::Ptr dataPtr(new Data);
            DataPtr data(dataPtr);
            TEST_TRUE(data());
        }
    }
    {
        LOG_SM(Unlock, "Unlock");
        DataPtr data(new Data);
        DataPtr::Unlock unlock(data);
        ++unlock->value_;
        {
            DataPtr::Unlock unlock2(data);
            ++unlock2->value_;
        }
    }
    {
        LOG_SM(MultiThreaded, "MultiThreaded");
        const int NrThreads = 10;
        DataPtr data(new Data);
        for (int i = 0; i < NrThreads; ++i)
            new IncrementThread<DataPtr>(data);
        while (IncrementThread<DataPtr>::nrInstances() > 0)
            gubg::nanosleep(0, 100000000);
        DataPtr::Unlock unlock(data);
        TEST_EQ(NrThreads, unlock->value_);
    }
    {
        LOG_SM(Singleton, "Mutex per instance");
        DataPtr data1(new Data);
        DataPtr data2(new Data);
        new IncrementThread<DataPtr>(data1);
        new IncrementThread<DataPtr>(data2);
        while (IncrementThread<DataPtr>::nrInstances() > 0)
            gubg::nanosleep(0, 100000000);
    }
    {
        LOG_SM(Singleton, "Mutex per type");
        SDataPtr data1(new Data);
        SDataPtr data2(new Data);
        new IncrementThread<SDataPtr>(data1);
        new IncrementThread<SDataPtr>(data2);
        while (IncrementThread<SDataPtr>::nrInstances() > 0)
            gubg::nanosleep(0, 100000000);
    }

    return 0;
}
