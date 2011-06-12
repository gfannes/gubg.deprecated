#include "pointer/Locked.hpp"
#include "testing/Testing.hpp"
#define GUBG_LOG
#include "logging/Log.hpp"
using namespace gubg::pointer;

namespace
{
    class Data
    {
        public:
            typedef boost::shared_ptr<Data> Ptr;

            Data():
                value_(0)
        {
            LOG_SM(Data, "constructor");
        }
            ~Data()
            {
                LOG_SM(Data, "destructor, value_: " << value_);
            }

            int value_;
    };
}

int main()
{
    TEST_TAG(main);
    LOG_S(main);
    typedef Locked<Data, ThreadSafeUnlocker> DataPtr;
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

    return 0;
}
