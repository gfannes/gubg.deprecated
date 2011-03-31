#include "testing/Testing.hpp"
#include "OnlyOnce.hpp"
#include "boost/thread/mutex.hpp"
#include <iostream>
using namespace std;

//#define L_ENABLE_DEBUG
#include "debug.hpp"

std::ostream &operator<<(std::ostream &os, const gubg::SourceLocation &location)
{
    return os << location.filename << ":" << location.lineNr;
}

namespace
{
    gubg::TestMgrPtr testMgr;
    boost::mutex testMgrMutex;
}

namespace gubg
{
    TestMgr &TestMgr::instance()
    {
        boost::mutex::scoped_lock lock(testMgrMutex);
        if (!testMgr)
            testMgr.reset(new TestMgr);
        return *testMgr;
    }

    TestMgr::TestMgr():
        currentStats_(0)
    {
        DEBUG_PRINT("TestMgr is initialized");
        syncCurrentStatistics_();
    }
    TestMgr::~TestMgr()
    {
        DEBUG_PRINT("TestMgr is finalized");
#if 0
        for (const auto &statsPerCase: statsPerSuite_)
        {
            OnlyOnce printSuite;
            for (const auto &statsPerPart: statsPerCase.second)
            {
                OnlyOnce printCase;
                for (const auto &stats: statsPerPart.second)
                {
                    const Statistics &s = stats.second;
                    if (s.total() == 0)
                        continue;
                    if (printSuite())
                        cout << "Suite(" << statsPerCase.first << ")" << endl;
                    if (printCase())
                        cout << "  Case(" << statsPerPart.first << ")" << endl;
                    cout << "    Part(" << stats.first << ") ";
                    if (0 == s.nrFailure)
                        cout << "Success, all " << s.nrSuccess << " tests passed." << endl;
                    else
                        cout << "FAILURE, " << s.nrFailure << " out of " << s.total() << " tests failed." << endl;
                }
            }
        }
#else
        for (const auto &suite: suiteOrder_)
        {
            OnlyOnce printSuite;
            auto caseOrder = caseOrderPerSuite_[suite];
            for (const auto &cas: caseOrder)
            {
                OnlyOnce printCase;
                auto partOrder = partOrderPerSuiteCase_[make_pair(suite, cas)];
                for (const auto &part: partOrder)
                {
                    const Statistics &s = statsPerSuite_[suite][cas][part];
                    if (s.total() == 0)
                        continue;
                    if (printSuite())
                        cout << "Suite(" << suite << ")" << endl;
                    if (printCase())
                        cout << "  Case(" << cas << ")" << endl;
                    cout << "    Part(" << part << ") ";
                    if (0 == s.nrFailure)
                        cout << "Success, all " << s.nrSuccess << " tests passed." << endl;
                    else
                        cout << "FAILURE, " << s.nrFailure << " out of " << s.total() << " tests failed." << endl;
                }
            }
        }
#endif
    }

    void TestMgr::success()
    {
        boost::mutex::scoped_lock lock(testMgrMutex);
        ++(currentStats_->nrSuccess);
    }
    void TestMgr::failure()
    {
        boost::mutex::scoped_lock lock(testMgrMutex);
        ++(currentStats_->nrFailure);
    }

    TestTag TestMgr::setTestTag(TestTag testTag)
    {
        TestTag res = currentTag_;
        currentTag_ = testTag;
        syncCurrentStatistics_();
        return res;
    }

    //Private methods
    void TestMgr::syncCurrentStatistics_()
    {
        if (!statsPerSuite_.count(currentTag_.testSuite))
            suiteOrder_.push_back(currentTag_.testSuite);
        StatsPerCase &statsPerCase = statsPerSuite_[currentTag_.testSuite];
        Order &caseOrder = caseOrderPerSuite_[currentTag_.testSuite];

        if (!statsPerCase.count(currentTag_.testCase))
            caseOrder.push_back(currentTag_.testCase);
        StatsPerPart &statsPerPart = statsPerCase[currentTag_.testCase];
        Order &partOrder = partOrderPerSuiteCase_[std::make_pair(currentTag_.testSuite, currentTag_.testCase)];

        if (!statsPerPart.count(currentTag_.testPart))
            partOrder.push_back(currentTag_.testPart);
        Statistics &statistics = statsPerPart[currentTag_.testPart];
        currentStats_ = &statistics;
    }
}

#ifdef UnitTest
using namespace gubg;
int main()
{
    {
        TEST_SUITE(test_positives);
        {
            TEST_CASE(test_eq);
            const unsigned int NrTests = 10;
            const string str("test123");
            for (unsigned int i = 0; i < NrTests; ++i)
            {
                {
                    TEST_PART(integers);
                    test_eq(1, 1, HERE());
                    test_eq(1, 2, HERE());
                    TEST_EQ(1, 3);
                }
                {
                    TEST_PART(strings);
                    TEST_EQ("test123", str);
                    TEST_EQ(str, "test123");
                }
            }
        }
        {
            TEST_CASE(test_true);
            TEST_TRUE(true);
            TEST_TRUE(false);
        }
    }
    {
        TEST_SUITE(test_negatives);
        {
            TEST_CASE(test_neq);
            const unsigned int NrTests = 10;
            const string str("test123");
            for (unsigned int i = 0; i < NrTests; ++i)
            {
                {
                    TEST_PART(integers);
                    test_neq(1, 1, HERE());
                    test_neq(1, 2, HERE());
                    TEST_NEQ(1, 3);
                }
                {
                    TEST_PART(strings);
                    TEST_NEQ("test123", str);
                    TEST_NEQ(str, "test123");
                }
            }
        }
        {
            TEST_CASE(test_false);
            TEST_FALSE(true);
            TEST_FALSE(false);
        }
    }
}
#endif
