#include "gubg/testing/Testing.hpp"
#include "gubg/Exception.hpp"
#include "gubg/OnlyOnce.hpp"
#include <iostream>
#include <sstream>
using namespace std;

std::ostream &operator<<(std::ostream &os, const gubg::testing::SourceLocation &location)
{
    return os << location.filename << ":" << location.lineNr;
}
std::ostream &operator<<(std::ostream &os, const std::nullptr_t &ptr)
{
    return os << "nullptr";
}

namespace
{
    template <typename T>
    string toStringValue_(const T &t)
    {
        ostringstream oss;
        oss << t;
        return oss.str();
    }
}

namespace gubg
{
    namespace testing
    {
        template <>
            std::string toString_<nullptr_t>(const nullptr_t &p){return "nullptr";}

        template <>
            bool areEqual<char, char>(const std::string &lhs, const std::string &rhs)
            {
                return lhs == rhs;
            }
        template <>
            bool areEqual<unsigned char, unsigned char>(const std::basic_string<unsigned char> &lhs, const std::basic_string<unsigned char> &rhs)
            {
                return lhs == rhs;
            }
        bool isPrintable(char ch)
        {
            if ('a' <= ch && ch <= 'z')
                return true;
            if ('A' <= ch && ch <= 'Z')
                return true;
            return false;
        }
        std::string toString_(const std::string &str)
        {
            std::ostringstream oss;
            std::ostringstream ossHex;
            OnlyOnce skipPipe;
            int nrHex = 0;
            oss << '"';
            for (auto it = str.begin(); it != str.end(); ++it)
            {
                char ch = *it;
                bool ip = isPrintable(ch);
                if (!ip)
                    ++nrHex;
                oss <<  (ip ? ch : '.');
                ossHex << (skipPipe() ? "" : "|") << std::hex << std::setw(2) << std::setfill('0') << (0xff&(unsigned int)ch);
            }
            oss << '"';
            if (nrHex)
                oss << " (" << ossHex.str() << ")";
            return oss.str();
        }
    }
}

#ifdef UnitTest
#include "threading/InstanceCounter.hpp"
#include "boost/thread/mutex.hpp"
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
    {
        TEST_TAG(nullptr);
        TEST_EQ(nullptr, nullptr);
        int *p = 0;
        TEST_EQ(nullptr, p);
        TEST_EQ(p, nullptr);
        int i;
        p = &i;
        cout << "p: " << p << ", &p: " << &p << endl;
        TEST_NEQ(nullptr, p);
        TEST_NEQ(p, nullptr);

        TEST_EQ(nullptr, p);
        TEST_EQ(p, nullptr);
    }
    TEST_REPORT_TYPE(Full);
    TEST_REPORT();

    struct Thread: gubg::threading::InstanceCounter<Thread>
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
    //When the program closes, another test report will be presented. Make sure we don't show the 1000 successes from the threads in Full
}
#endif
