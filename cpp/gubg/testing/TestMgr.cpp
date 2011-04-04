#include "testing/TestMgr.hpp"
#include "boost/thread/tss.hpp"
#include <mutex>
using namespace gubg::testing;
using namespace std;

#define L_ENABLE_DEBUG
#include "debug.hpp"

namespace
{
    TestMaster *testMaster = nullptr;
    mutex testMasterMutex;

    //This will hold the thread-specific root tag
    void doNothing(gubg::testing::TestTag*){}
    boost::thread_specific_ptr<TestTag> rootTag(doNothing);
}

//Statistics methods
Statistics::Statistics():
    nrSuccess(0),
    nrFailure(0){}
void Statistics::addResult(TestResult result)
{
    switch (result)
    {
        case TestResult::Success: ++nrSuccess; break;
        case TestResult::Failure: ++nrFailure; break;
    }
}
unsigned int Statistics::nrTotal() const
{
    return nrSuccess + nrFailure;
}

//TestTag methods
TestTag::TestTag(const string &tag):
    tag_(tag),
    root_(nullptr)
{
    DEBUG_PRINT("ctor " << tag_);
    if (nullptr == rootTag.get())
    {
        DEBUG_PRINT("We are the root tag");
        rootTag.reset(this);
        root_ = new ThreadStats;
    }
    else
    {
        root_ = rootTag.get()->root_;
    }
    root_->addTag(tag_);
}
TestTag::~TestTag()
{
    DEBUG_PRINT("dtor " << tag_);
    root_->popTag();
    if (this == rootTag.get())
    {
        DEBUG_PRINT("We were the root tag");
        TestMaster::instance().report(*(const ThreadStats *)root_);
        delete root_;
        rootTag.reset(nullptr);
    }
}
void TestTag::addResult(TestResult result)
{
    root_->addResult(result);
}
//TestTag::ThreadStats methods
TestTag::ThreadStats::ThreadStats():
    currentStats_(nullptr){}
void TestTag::ThreadStats::addTag(const std::string &tag)
{
    tags_.push_back(tag);
    currentStats_ = &statsPerTags_[tags_];
}
void TestTag::ThreadStats::popTag()
{
    tags_.pop_back();
    currentStats_ = (tags_.empty() ? nullptr : &statsPerTags_[tags_]);
}
void TestTag::ThreadStats::addResult(TestResult result)
{
    currentStats_->addResult(result);
}

//TestMaster methods
TestMaster &TestMaster::instance()
{
    lock_guard<mutex> lock(testMasterMutex);
    if (nullptr == testMaster)
        testMaster = new TestMaster;
    return *testMaster;
}
void TestMaster::report(const TestTag::ThreadStats &threadStats)
{
    DEBUG_PRINT("Someone is reporting thread statistics");
    lock_guard<mutex> lock(testMasterMutex);
    threadStatss_.push_back(threadStats);
}
std::ostream &operator<<(std::ostream &os, const TestMaster &testMaster)
{
    os << "I received results from " << testMaster.threadStatss_.size() << " thread(s)" << endl;
    Statistics stats;
    for (const auto &threadStats: testMaster.threadStatss_)
    {
        for (const auto &v: threadStats.statsPerTags_)
        {
            const TestTag::ThreadStats::Tags &tags =  v.first;
            const Statistics &stats = v.second;
        }
    }
    return os;
}

#ifdef UnitTest
int main()
{
    {
        TestTag root("root");
        TestTag level1("level1");
        TestTag level2("level2");
        level2.addResult(TestResult::Success);
    }
    cout << TestMaster::instance() << endl;
}
#endif
