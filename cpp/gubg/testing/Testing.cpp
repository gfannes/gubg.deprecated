#include "testing/Testing.hpp"
#include "Exception.hpp"
#include "OnlyOnce.hpp"
#include "threading/Thread.hpp"
#include "boost/thread/mutex.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;

//#define L_ENABLE_DEBUG
#include "debug.hpp"

std::ostream &operator<<(std::ostream &os, const gubg::SourceLocation &location)
{
    return os << location.filename << ":" << location.lineNr;
}

namespace gubg
{
    string toHex(const string &binary)
    {
        ostringstream res;
        res << hex;
        for (auto byte: binary)
            res << "0x" << setw(2) << setfill('0') << (int)(0xff & byte) << ", ";
        return res.str();
    }
}

#ifdef UnitTest
using namespace gubg;
int main()
{
    {
        TEST_TAG(test_positives);
        {
            TEST_TAG(test_eq);
            const unsigned int NrTests = 2;
            const string str("test123");
            for (unsigned int i = 0; i < NrTests; ++i)
            {
                {
                    gubg::testing::TestTag tag("integers");
                    test_eq(1, 1, HERE(), tag);
                    test_eq(1, 2, HERE(), tag);
                    TEST_EQ(1, 3);
                }
                {
                    TEST_TAG(strings);
                    TEST_EQ("test123", str);
                    TEST_EQ(str, "test123");
                }
            }
        }
        {
            TEST_TAG(test_true);
            TEST_TRUE(true);
            TEST_TRUE(false);
        }
    }
    {
        TEST_TAG(test_negatives);
        {
            TEST_TAG(test_neq);
            const string str("test123");
            {
                gubg::testing::TestTag tag("integers");
                test_neq(1, 1, HERE(), tag);
                test_neq(1, 2, HERE(), tag);
                TEST_NEQ(1, 3);
            }
            {
                TEST_TAG(strings);
                TEST_NEQ("test123", str);
                TEST_NEQ(str, "test123");
            }
        }
        {
            TEST_TAG(test_false);
            TEST_FALSE(true);
            TEST_FALSE(false);
        }
    }
    {
        TEST_TAG(exceptions);
        TEST_THROW(gubg::Exception, throw gubg::Exception(""));
    }
    TEST_REPORT_TYPE(Full);
    TEST_REPORT();

    struct Thread: gubg::threading::Thread<Thread>
    {
        Thread():thread_(boost::ref(*this)){}
        void operator()()
        {
            TEST_TAG(Thread);
            TEST_TRUE(true);
            delete this;
        }
        boost::thread thread_;
    };
    for (unsigned int i = 0; i < 1000; ++i)
        new Thread;

    while (Thread::nrInstances() > 0){}

    TEST_REPORT_TYPE(OnlyErrors);
    //We the program closes, another test report will be presented. Make sure we don't show the 1000 successes from the threads in Full
}
#endif
