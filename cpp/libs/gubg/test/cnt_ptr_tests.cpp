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

    return 0;
}
#include "gubg/log/end.hpp"
