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
    typedef Locked<Data, ThreadSafeInstance> DataPtrI;
    typedef Locked<Data, ThreadSafeType> DataPtrT;

    template <typename DataPtrI>
    struct IncrementThread: gubg::InstanceCounter<IncrementThread<DataPtrI>>
    {
        IncrementThread(DataPtrI data):
            data_(data),
            thread_(boost::ref(*this)){}
        void operator()()
        {
            typename DataPtrI::Unlock unlock(data_);
            int v = unlock->value_;
            gubg::nanosleep(0, 100000000);
            unlock->value_ = v+1;
            delete this;
        }
        DataPtrI data_;
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
            DataPtrI data(new Data);
            TEST_TRUE(data());
        }
        {
            LOG_SM(SharedPointer, "SharedPointer");
            Data::Ptr dataPtr(new Data);
            DataPtrI data(dataPtr);
            TEST_TRUE(data());
        }
    }
    {
        LOG_SM(Unlock, "Unlock");
        DataPtrI data(new Data);
        DataPtrI::Unlock unlock(data);
        ++unlock->value_;
        {
            DataPtrI::Unlock unlock2(data);
            ++unlock2->value_;
        }
    }
    {
        LOG_SM(MultiThreaded, "MultiThreaded");
        const int NrThreads = 10;
        DataPtrI data(new Data);
        for (int i = 0; i < NrThreads; ++i)
            new IncrementThread<DataPtrI>(data);
        while (IncrementThread<DataPtrI>::nrInstances() > 0)
            gubg::nanosleep(0, 100000000);
        DataPtrI::Unlock unlock(data);
        TEST_EQ(NrThreads, unlock->value_);
    }
    {
        LOG_SM(Singleton, "Mutex per instance");
        DataPtrI data1(new Data);
        DataPtrI data2(new Data);
        new IncrementThread<DataPtrI>(data1);
        new IncrementThread<DataPtrI>(data2);
        while (IncrementThread<DataPtrI>::nrInstances() > 0)
            gubg::nanosleep(0, 100000000);
    }
    {
        LOG_SM(Singleton, "Mutex per type");
        DataPtrT data1(new Data);
        DataPtrT data2(new Data);
        new IncrementThread<DataPtrT>(data1);
        new IncrementThread<DataPtrT>(data2);
        while (IncrementThread<DataPtrT>::nrInstances() > 0)
            gubg::nanosleep(0, 100000000);
    }

    return 0;
}
