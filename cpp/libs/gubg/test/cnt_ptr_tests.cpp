#include "gubg/cnt_ptr.hpp"
#include "gubg/threading/InstanceCounter.hpp"
#include "gubg/Testing.hpp"

namespace  { 
    struct Data: gubg::InstanceCounter<Data>
    {
        int i;
    };
} 

#define GUBG_MODULE_ "test"
#include "gubg/log/begin.hpp"
int main()
{
    TEST_TAG(main);

    typedef gubg::cnt_ptr<Data> Ptr;
    typedef gubg::track_ptr<Data> WPtr;
    L(STREAM(sizeof(Ptr)));

    {
        Ptr ptr;
        TEST_FALSE(ptr);
    }
    TEST_EQ(0, Data::nrInstances());

    {
        Ptr ptr(new Data);
        TEST_TRUE(ptr);
        TEST_EQ(1, Data::nrInstances());
        ptr.reset();
        TEST_EQ(0, Data::nrInstances());
        TEST_FALSE(ptr);
        ptr.reset(new Data);
        TEST_EQ(1, Data::nrInstances());
        TEST_TRUE(ptr);
    }
    TEST_EQ(0, Data::nrInstances());

    {
        Ptr ptr(new Data);
        TEST_EQ(1, Data::nrInstances());
        Ptr ptr2 = ptr;
        TEST_EQ(1, Data::nrInstances());
        ptr.reset();
        TEST_EQ(1, Data::nrInstances());
        ptr2.reset();
        TEST_EQ(0, Data::nrInstances());
    }
    TEST_EQ(0, Data::nrInstances());

    {
        Ptr ptr(new Data);
        Ptr ptr2(ptr);
        ptr->i = 42;
        ptr.reset();
        TEST_EQ(42, ptr2->i);
    }
    TEST_EQ(0, Data::nrInstances());

    {
        Ptr ptr(new Data);
        Ptr ptr2(ptr);
        (*ptr).i = 42;
        ptr.reset();
        TEST_EQ(42, (*ptr2).i);
    }
    TEST_EQ(0, Data::nrInstances());

    {
        Ptr ptr(new Data);
        ptr->i = 42;
        WPtr wp;
        WPtr wptr(ptr);
        TEST_TRUE(wptr.lock());
        {
            Ptr ptr2 = wptr.lock();
            ptr2->i = 1234;
        }
        TEST_EQ(1234, ptr->i);
        ptr.reset();
        TEST_FALSE(wptr.lock());
    }
    TEST_EQ(0, Data::nrInstances());

    return 0;
}
#include "gubg/log/end.hpp"
